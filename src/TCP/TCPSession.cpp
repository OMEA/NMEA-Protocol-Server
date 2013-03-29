//
//  TCPSession.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "TCPSession.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/system/system_error.hpp>


#include "icmp_header.hpp"
#include "ipv4_header.hpp"

#if defined __linux__
#include <sys/types.h>
#include <sys/socket.h>
typedef int SOCKET;
#define INVALID_SOCKET -1
#define SOCKET_ERROR   -1
#endif

using boost::asio::ip::tcp;

TCPSession* TCPSession::factory(boost::shared_ptr<Endpoint> connectedTo, boost::asio::io_service& io_service, unsigned int port) {
    TCPSession* tcpSession = new TCPSession(connectedTo, io_service, port);
    tcpSession->initialize();
    return tcpSession;
}

void TCPSession::initialize(){
    AsyncEndpoint<boost::asio::ip::tcp::socket>::initialize();
    registerUIntCmd("icmp_timeouts","ICMP Keepalive protection timeouts", "Defines the number of allowed timeouts. -1 means infinite and turns of the keep-alive packets", &icmp_maxtimeouts, 5, 1, 3600, true);
    boost::function<void (Command_ptr)> func = boost::bind(&TCPSession::enable_icmp_cmd, this, _1);
    registerVoidCmd("icmp","ICMP Keepalive protection", "Enables or disables the ICMP keepalive protection. Allowed parameters are on or off. The command checks whether ICMP is enabled on the remote host before enabling.",  func);
    unregisterCmd("print_stats");
    boost::function<void (Command_ptr)> func2 = boost::bind(&TCPSession::print_stats_cmd, this, _1);
    registerVoidCmd("print_stats","Print Session statistics", "Prints statistics about the session.",  func2);
    icmp_keepalive=false;
    stat_roundtrip_ms=-1;
    lost_icmp_packets=0;
}

void TCPSession::enable_icmp_cmd(Command_ptr command){
    if(command->getArguments()==""){
        if(icmp_keepalive){
            command->answer("ICMP Keepalive protection is enabled!\n", this->shared_from_this());
        }
        else{
            command->answer("ICMP Keepalive protection is disabled\n", this->shared_from_this());
        }
    }
    if(command->getArguments()=="on"){
        if(!icmp_keepalive){
            icmp_keepalive=true;
            workerThread=boost::thread(&TCPSession::icmp_setup, this);
            command->answer("ICMP Keepalive protection now is enabled!\n", this->shared_from_this());
        }
        else{
            command->answer("ICMP Keepalive protection already is enabled!\n", this->shared_from_this());
        }
    }
    else if(command->getArguments()=="off"){
        if(icmp_keepalive){
            icmp_keepalive=false;
            icmpsocket_.close();
            command->answer("ICMP Keepalive protection now is disabled!\n", this->shared_from_this());
        }
        else{
            command->answer("ICMP Keepalive protection already is disabled!\n", this->shared_from_this());
        }
    }
    else{
        command->answer(Answer::WRONG_ARGS, "Cannot understand command Argument "+command->getArguments()+" for Command "+command->getCommand()+"\n", this->shared_from_this());
    }
}

void TCPSession::print_stats_cmd(Command_ptr command){
    AsyncEndpoint<boost::asio::ip::tcp::socket>::print_stats_cmd(command);
    std::ostringstream oss;
    oss << "Current Roundtrip delay: " << '\t' << stat_roundtrip_ms << "ms" << std::endl;
    oss << "Lost ICMP Packets (total): " << '\t' << lost_icmp_packets << std::endl;
    command->answer(oss.str(), this->shared_from_this());
}

//TCPSession::TCPSession(boost::asio::io_service& io_service, unsigned int port): socket_(io_service), port(port), icmpsocket_(io_service, icmp::v4()), timer_(io_service), resolver_(io_service)
//{
//    boost::asio::socket_base::keep_alive keepAlive(true);
//    socket_.set_option(keepAlive);
////#if defined __linux__
////    SOCKET native_sock = socket_.native();
////    int result = -1;
////    
////    if (INVALID_SOCKET != native_sock)
////    {
////        int idle = 1;
////        result = setsockopt(native_sock, SOL_SOCKET, TCP_KEEPIDLE, &idle, sizeof(idle));
////        int interval = 1;
////        result = setsockopt(native_sock, SOL_SOCKET, TCP_KEEPINTVL, &interval, sizeof(interval));
////        int count = 10;
////        result = setsockopt(native_sock, SOL_SOCKET, TCP_KEEPCNT, &count, sizeof(count));
////        std::cout << "changed keepalive "<<result<<std::endl;
////    }
////    else{
////        std::cout << "invalid socket!"<<std::endl;
////    }
////#endif
//    setAOStream(&socket_);
//}

TCPSession::TCPSession(boost::shared_ptr<Endpoint> connectedTo, boost::asio::io_service& io_service, unsigned int port): AsyncEndpoint<boost::asio::ip::tcp::socket>(connectedTo), socket_(io_service), port(port), timer_(icmp_io_service), icmpsocket_(icmp_io_service), resolver_(icmp_io_service)
{
    //boost::asio::socket_base::keep_alive keepAlive(true);
    //socket_.set_option(keepAlive);
    setAOStream(&socket_);
}

TCPSession::~TCPSession(){
}

std::string TCPSession::getId(){
    std::ostringstream ss;
    try{
        boost::asio::ip::tcp::resolver resolver(socket_.get_io_service());
        boost::asio::ip::tcp::resolver::iterator host = resolver.resolve(socket_.remote_endpoint());
    
        ss<<host->host_name() << ":" << socket_.remote_endpoint().port()<< "@tcp"<<port;
    }
    catch (std::exception& e){
        ss<< "inactive_socket@tcp"<<port;
    }
    return ss.str();
}

tcp::socket& TCPSession::socket()
{
    return socket_;
}

void TCPSession::start()
{
    AsyncEndpoint<boost::asio::ip::tcp::socket>::start();
}

void TCPSession::stop()
{
    if(icmp_keepalive){
        icmp_keepalive=false;
        if(icmpsocket_.is_open()){
            icmpsocket_.close();
        }
        icmp_io_service.stop();
        workerThread.join();
    }
    AsyncEndpoint<boost::asio::ip::tcp::socket>::stop();
}

void TCPSession::icmp_setup(){
    try{
        icmp_first_try = true;
        icmp_timeouts=0;
        icmp_maxtimeouts=5;
        icmp::resolver::query query(icmp::v4(), socket().remote_endpoint().address().to_string(), "");
        icmp::endpoint destination_ = *resolver_.resolve(query);
        icmpsocket_.connect(destination_);
        icmp_test();
        icmp_receive();
    }
    catch (std::exception& e)
    {
        std::cerr << "TCP Exception: " << e.what() << "\n";
    }
    try{
        icmp_io_service.run();
    }
    catch (std::exception& e){}
}

void TCPSession::icmp_test(){
    
    std::string body("\"Hello!\" from Asio ping.");
    icmp_header echo_request;
    echo_request.type(icmp_header::echo_request);
    echo_request.code(0);
    echo_request.identifier(getpid());
    echo_request.sequence_number(++sequence_number_);
    compute_checksum(echo_request, body.begin(), body.end());
    
    // Encode the request packet.
    boost::asio::streambuf request_buffer;
    std::ostream os(&request_buffer);
    os << echo_request << body;
    
    // Send the request.
    time_sent_ = boost::posix_time::microsec_clock::universal_time();
    //icmpsocket_.send_to(request_buffer.data(), destination_);
    icmpsocket_.send(request_buffer.data());
    
    // Wait up to five seconds for a reply.
    num_replies_ = 0;
    timer_.expires_at(time_sent_ + boost::posix_time::seconds(1));
    timer_.async_wait(boost::bind(&TCPSession::icmp_timeout, this));
}

void TCPSession::icmp_timeout(){
    if (num_replies_ == 0){
        lost_icmp_packets++;
        if(icmp_first_try){
            std::cout << "It seems as ICMP is not enabled on the remote Host, icmp-keepalive disabled" << std::endl;
            icmp_keepalive = false;
            icmpsocket_.close();
        }
        else{
            std::cout << "Request timed out" << std::endl;
            icmp_timeouts++;
            if(icmp_timeouts>icmp_maxtimeouts){
                //stop();
                socket().close();
                return;
            }
        }
    }
    else{
        icmp_first_try=false;
        icmp_timeouts=0;
    }
    
    // Requests must be sent no less than one second apart.
    timer_.expires_at(time_sent_ + boost::posix_time::seconds(1));
    timer_.async_wait(boost::bind(&TCPSession::icmp_test, this));
}

void TCPSession::icmp_receive(){
    // Discard any data already in the buffer.
    reply_buffer_.consume(reply_buffer_.size());
    
    // Wait for a reply. We prepare the buffer to receive up to 64KB.
    icmpsocket_.async_receive(reply_buffer_.prepare(65536),
                          boost::bind(&TCPSession::icmp_handle_receive, this, _2));
}

void TCPSession::icmp_handle_receive(std::size_t length){
    // The actual number of bytes received is committed to the buffer so that we
    // can extract it using a std::istream object.
    reply_buffer_.commit(length);
    
    // Decode the reply packet.
    std::istream is(&reply_buffer_);
    ipv4_header ipv4_hdr;
    icmp_header icmp_hdr;
    is >> ipv4_hdr >> icmp_hdr;
    
    // We can receive all ICMP packets received by the host, so we need to
    // filter out only the echo replies that match the our identifier and
    // expected sequence number.
    if (is && icmp_hdr.type() == icmp_header::echo_reply
        && icmp_hdr.identifier() == getpid()
        && icmp_hdr.sequence_number() == sequence_number_)
    {
        // If this is the first reply, interrupt the five second timeout.
        if (num_replies_++ == 0)
            timer_.cancel();
        
        // Print out some information about the reply packet.
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
        stat_roundtrip_ms = (now - time_sent_).total_milliseconds();
        //std::cout << length - ipv4_hdr.header_length()
        //<< " bytes from " << ipv4_hdr.source_address()
        //<< ": icmp_seq=" << icmp_hdr.sequence_number()
        //<< ", ttl=" << ipv4_hdr.time_to_live()
        //<< ", time=" << (now - time_sent_).total_milliseconds() << " ms"
        //<< std::endl;
    }
    icmp_receive();
}
