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

class TCPServer: public CommandEndpoint, public boost::enable_shared_from_this<TCPServer>
{
public:
    static boost::shared_ptr<TCPServer> factory(boost::shared_ptr<Endpoint> connectedTo, short port=10110);
protected:
    TCPServer(boost::shared_ptr<Endpoint> connectedTo, boost::asio::io_service& io_service, short port);
public:    
    void handle_accept(TCPSession* new_session, const boost::system::error_code& error);
    
    virtual std::string getId();
protected:
    virtual void deliverAnswer_impl(Answer_ptr answer){}
    virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
private:
    std::list<TCPSession_ptr> sessions;
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::acceptor acceptor_;
    short port;
};

typedef boost::shared_ptr<TCPServer> TCPServer_ptr;


#endif /* defined(__NMEA_Protocol_Server__TCPServer__) */
