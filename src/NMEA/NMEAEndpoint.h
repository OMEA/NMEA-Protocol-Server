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

class NMEAEndpoint
{
public:
    NMEAEndpoint();
    virtual ~NMEAEndpoint(){}
    void deliver(NMEAmsg_ptr msg);
    void receive(NMEAmsg_ptr msg);
    virtual void registerEndpoint();
    virtual void unregisterEndpoint();
    virtual void receiveCommand(Command_ptr command);
    void deliverCommand(Command_ptr command);
    void deliverAnswer(std::string answer);
    virtual std::string getId()=0;
protected:
    virtual void deliver_impl(NMEAmsg_ptr msg)=0;
    virtual void deliverAnswer_impl(std::string msg)=0;
    virtual void receive_impl(NMEAmsg_ptr msg);
    virtual boost::shared_ptr<NMEAEndpoint> v_shared_from_this()=0;
    
protected:
    bool input;
    bool output;
    bool portmirror;
    bool checksum;
};

typedef boost::shared_ptr<NMEAEndpoint> NMEAEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__NMEAEndpoint__) */
