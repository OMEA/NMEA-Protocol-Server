//
//  NMEAEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#ifndef __NMEA_Protocol_Server__NMEAEndpoint__
#define __NMEA_Protocol_Server__NMEAEndpoint__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "NMEAmsg.h"
#include "../Support/Command.h"
#include "../Support/Endpoint.h"

class NMEAEndpoint: public CommandEndpoint
{
public:
    using CommandEndpoint::deliver;
    using CommandEndpoint::receive;
    
public:
    NMEAEndpoint();
    virtual ~NMEAEndpoint(){}
    virtual void deliver(NMEAmsg_ptr msg);
    virtual void receive(NMEAmsg_ptr msg);
    
    virtual void receive(Command_ptr command);
protected:
    virtual void deliver_impl(NMEAmsg_ptr msg)=0;
    virtual void receive_impl(NMEAmsg_ptr msg);
    virtual boost::shared_ptr<Endpoint> v_shared_from_this()=0;
    
protected:
    bool input;
    bool output;
    bool portmirror;
    bool checksum;
};

typedef boost::shared_ptr<NMEAEndpoint> NMEAEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__NMEAEndpoint__) */
