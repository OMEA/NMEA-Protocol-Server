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


#include <boost/thread.hpp>

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

TCPSession::TCPSession(boost::asio::io_service& io_service, unsigned int port): socket_(io_service), port(port)
{
    boost::asio::socket_base::keep_alive keepAlive(true);
    socket_.set_option(keepAlive);
#if defined __linux__
    SOCKET native_sock = socket_.native();
    int result = -1;
    
    if (INVALID_SOCKET != native_sock)
    {
        int idle = 1;
        result = setsockopt(native_sock, SOL_SOCKET, TCP_KEEPIDLE, &idle, sizeof(idle));
        int interval = 1;
        result = setsockopt(native_sock, SOL_SOCKET, TCP_KEEPINTVL, &interval, sizeof(interval));
        int count = 10;
        result = setsockopt(native_sock, SOL_SOCKET, TCP_KEEPCNT, &count, sizeof(count));
        std::cout << "changed keepalive "<<result<<std::endl;
    }
    else{
        std::cout << "invalid socket!"<<std::endl;
    }
#endif
    setAOStream(&socket_);
}

TCPSession::TCPSession(boost::shared_ptr<Endpoint> connectedTo, boost::asio::io_service& io_service, unsigned int port): AsyncEndpoint<boost::asio::ip::tcp::socket>(connectedTo), socket_(io_service), port(port)
{
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
