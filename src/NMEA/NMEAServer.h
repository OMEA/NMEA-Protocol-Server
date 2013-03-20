//
//  Server.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 22.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#ifndef __NMEA_Protocol_Server__Server__
#define __NMEA_Protocol_Server__Server__

#include <iostream>
#include <queue>
#include <list>
#include <boost/thread/condition_variable.hpp>

#include "../Support/Command.h"
#include "../Support/IOServicePool.h"
#include "NMEAmsg.h"
#include "NMEAMidpoint.h"

class NMEAServer;
typedef boost::shared_ptr<NMEAServer> NMEAServer_ptr;

class NMEAServer: public NMEAMidpoint, public boost::enable_shared_from_this<NMEAServer>
{
public:
    using NMEAEndpoint::deliver;
    using NMEAMidpoint::deliver;
private:
    std::list<Message_ptr> msgs;
    boost::condition_variable msgsCond;
    boost::mutex msgsMutex;
    std::list<Endpoint_ptr> online;
    boost::mutex onlineMutex;
    std::list<Endpoint_ptr> offline;
    boost::mutex offlineMutex;
    
    /// The pool of io_service objects used to perform asynchronous operations.
    io_service_pool io_service_pool_;
    bool shouldRun;
    void receiveCommand(Command_ptr command);
    
    void receive(Message_ptr msg);
    
    NMEAServer(void);
protected:
    virtual void deliver_impl(NMEAmsg_ptr msg){std::cerr<<"NMEAServer::deliver_impl not_implemented"<<std::endl;}
    virtual void deliverAnswer_impl(Answer_ptr answer){std::cerr<<"NMEAServer::deliverAnswer_impl not_implemented"<<std::endl;}
    virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
public:
    void run(void);
    void stop(void);
    virtual void receive(Command_ptr msg);
    virtual void receive(NMEAmsg_ptr msg);
    virtual void receive(Answer_ptr msg);
    virtual void deliver(Command_ptr command);
    
    virtual std::string getId(){return std::string("server");}
    ~NMEAServer(void){}
public:
    virtual void addEndpoint(Endpoint_ptr endpoint);
    virtual void removeEndpoint(Endpoint_ptr endpoint);
    virtual void endpointOnline(Endpoint_ptr endpoint);
    virtual void endpointOffline(Endpoint_ptr endpoint);
    
public:
    static NMEAServer_ptr getInstance();

};


#endif /* defined(__NMEA_Protocol_Server__Server__) */
