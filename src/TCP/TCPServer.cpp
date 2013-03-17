//
//  TCPServer.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "TCPServer.h"


using boost::asio::ip::tcp;

TCPServer::TCPServer(boost::asio::io_service& io_service, short port):
    io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
    port(port)
{
    TCPSession* new_session = new TCPSession(io_service_, port);
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&TCPServer::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));
}

void TCPServer::handle_accept(TCPSession* new_session, const boost::system::error_code& error)
{
    if (!error)
    {
        boost::asio::ip::tcp::resolver resolver(new_session->socket().get_io_service());
        boost::asio::ip::tcp::resolver::iterator host = resolver.resolve(new_session->socket().remote_endpoint());
        
        std::stringstream ss;
        ss<<host->host_name() << ":" << "@tcp"<<port;
        new_session->setSessionId(ss.str());
        
        NMEAEndpoint_ptr ptr(new_session);
        new_session->start();
        new_session = new TCPSession(io_service_, port);
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&TCPServer::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }
    else
    {
        delete new_session;
    }
}