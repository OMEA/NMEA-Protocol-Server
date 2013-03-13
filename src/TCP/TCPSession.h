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

#include "../Support/AsyncEndpoint.h"

using boost::asio::ip::tcp;

class TCPSession : public AsyncEndpoint<boost::asio::ip::tcp::socket>
{
public:
    TCPSession(boost::asio::io_service& io_service, unsigned int port);
    virtual ~TCPSession();
    
    tcp::socket& socket();
    
    std::string getId();
    
private:
    tcp::socket socket_;
    unsigned int port;
};


#endif /* defined(__NMEA_Protocol_Server__TCPSession__) */
