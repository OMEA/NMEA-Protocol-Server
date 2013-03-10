//
//  TCPSession.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "TCPSession.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/system/system_error.hpp>

#include <boost/thread.hpp>

using boost::asio::ip::tcp;

TCPSession::TCPSession(boost::asio::io_service& io_service): socket_(io_service)
{
    setAOStream(&socket_);
}

TCPSession::~TCPSession(){
    
}

std::string TCPSession::getId(){
    std::stringstream ss;
    ss<<socket_.remote_endpoint();
    return ss.str();
}

tcp::socket& TCPSession::socket()
{
    return socket_;
}

