//
//  TCPServer.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#ifndef __NMEA_Protocol_Server__TCPServer__
#define __NMEA_Protocol_Server__TCPServer__

#include <iostream>
#include <list>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include "TCPSession.h"


using boost::asio::ip::tcp;

class TCPServer
{
public:
    TCPServer(boost::asio::io_service& io_service, short port);
    
    void handle_accept(TCPSession* new_session, const boost::system::error_code& error);
    
private:
    std::list<TCPSession_ptr> sessions;
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    short port;
};


#endif /* defined(__NMEA_Protocol_Server__TCPServer__) */
