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
#include "NMEAmsg.h"
#include "NMEAEndpoint.h"

class NMEAServer
{
private:
    std::queue<NMEAmsg_ptr> msgs;
    std::list<NMEAEndpoint_ptr> endpoints;
    boost::condition_variable msgsCond;
    boost::mutex msgsMutex;
    
    NMEAServer(void){}
    ~NMEAServer(void){}
public:
    void run(void);
    void receive(NMEAmsg_ptr msg);
    void receiveCommand(Command_ptr command);
    void addEndpoint(NMEAEndpoint_ptr endpoint);
    void removeEndpoint(NMEAEndpoint_ptr endpoint);
public:
    static NMEAServer *getInstance();

};



#endif /* defined(__NMEA_Protocol_Server__Server__) */
