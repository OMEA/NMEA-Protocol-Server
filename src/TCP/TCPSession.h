//
//  TCPSession.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#ifndef __NMEA_Protocol_Server__TCPSession__
#define __NMEA_Protocol_Server__TCPSession__

#include <iostream>
#include <boost/thread.hpp>

#include "../Support/AsyncEndpoint.h"

using boost::asio::ip::tcp;
using boost::asio::ip::icmp;

class TCPSession : public AsyncEndpoint<boost::asio::ip::tcp::socket>
{
public:
    static TCPSession* factory(boost::shared_ptr<Endpoint> connectedTo, boost::asio::io_service& io_service, unsigned int port);
protected:
    //TCPSession(boost::asio::io_service& io_service, unsigned int port);
    TCPSession(boost::shared_ptr<Endpoint> connectedTo, boost::asio::io_service& io_service, unsigned int port);
public:
    virtual ~TCPSession();
    
    tcp::socket& socket();

    std::string getId();
    virtual void start();
    virtual void stop();
    virtual void initialize();
private:
    void enable_icmp_cmd(Command_ptr command);
private:
    tcp::socket socket_;
    unsigned int port;
private:
    void icmp_setup();
    void icmp_test();
    void icmp_receive();
    void icmp_handle_receive(std::size_t length);
    void icmp_timeout();
    boost::thread workerThread;
    boost::asio::io_service icmp_io_service;
    boost::asio::deadline_timer timer_;
    icmp::socket icmpsocket_;
    icmp::resolver resolver_;
    unsigned short sequence_number_;
    boost::posix_time::ptime time_sent_;
    boost::asio::streambuf reply_buffer_;
    std::size_t num_replies_;
    unsigned int icmp_timeouts;
    unsigned int icmp_maxtimeouts;
    bool icmp_first_try;
    bool icmp_keepalive;
};

typedef boost::shared_ptr<TCPSession> TCPSession_ptr;

#endif /* defined(__NMEA_Protocol_Server__TCPSession__) */
