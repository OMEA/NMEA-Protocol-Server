//
//  TCPServer.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "TCPServer.h"

#include <boost/thread.hpp>

using boost::asio::ip::tcp;

boost::shared_ptr<TCPServer> TCPServer::factory(boost::shared_ptr<Endpoint> connectedTo, short port) {
    //IOService in endpoint und beim beenden stop() aufrufen
    boost::asio::io_service *io_service = new boost::asio::io_service();
    
    TCPServer_ptr newServer(new TCPServer(connectedTo, *io_service, port));
    //FileEndpoint::factory(args);
    boost::thread bt(boost::bind(&boost::asio::io_service::run, io_service));
    newServer->registerEndpoint();
    return newServer;
}

TCPServer::TCPServer(boost::shared_ptr<Endpoint> connectedTo, boost::asio::io_service& io_service, short port):
CommandEndpoint(connectedTo),
io_service_(io_service),
acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
port(port)
{
    TCPSession* new_session = TCPSession::factory(connectedTo, io_service, port);
    acceptor_.async_accept(new_session->socket(),
                           boost::bind(&TCPServer::handle_accept, this, new_session,
                                       boost::asio::placeholders::error));
}


std::string TCPServer::getId(){
    std::ostringstream ss;
    ss << "tcp" << port;
    return ss.str();
}

void TCPServer::handle_accept(TCPSession* new_session, const boost::system::error_code& error)
{
    if (!error)
    {
        std::ostringstream ss;
        try{
            boost::asio::ip::tcp::resolver resolver(new_session->socket().get_io_service());
            boost::asio::ip::tcp::resolver::iterator host = resolver.resolve(new_session->socket().remote_endpoint());
        
            ss<<host->host_name() << ":" << "@tcp"<<port;
        }
        catch (std::exception& e)
        {
            ss<<new_session->socket().remote_endpoint().address().to_string() << ":" << "@tcp"<<port;
        }
        new_session->setSessionId(ss.str());
        
        NMEAEndpoint_ptr ptr(new_session);
        new_session->start();
        TCPSession* new_session = TCPSession::factory(getConnectedTo(), io_service_, port);
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&TCPServer::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }
    else
    {
        delete new_session;
    }
}