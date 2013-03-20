//
//  NMEAMidpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 19.03.13.
//
//

#ifndef __NMEA_Protocol_Server__NMEAMidpoint__
#define __NMEA_Protocol_Server__NMEAMidpoint__

#include <iostream>
#include "NMEAEndpoint.h"

class NMEAMidpoint: public NMEAEndpoint{
public:
    using NMEAEndpoint::deliver;
    using NMEAEndpoint::receive;
public:
    static boost::shared_ptr<NMEAMidpoint> factory(boost::shared_ptr<Endpoint> connectedTo, std::string midpointClass);
protected:
    NMEAMidpoint(boost::shared_ptr<Endpoint> connectedTo);
    NMEAMidpoint();
    virtual void initialize();
public:
    ~NMEAMidpoint(){}
    virtual void addEndpoint(Endpoint_ptr endpoint);
    virtual void removeEndpoint(Endpoint_ptr endpoint);
    virtual void endpointOnline(Endpoint_ptr endpoint);
    virtual void endpointOffline(Endpoint_ptr endpoint);
    std::string getId();
public:
    virtual void receive(Command_ptr command);
    virtual void deliver(Answer_ptr answer);
protected:
    virtual void deliver_impl(NMEAmsg_ptr msg);
    virtual void receive_impl(NMEAmsg_ptr msg);
    virtual void deliverAnswer_impl(Answer_ptr answer){}
    //virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
private:
    Endpoint_ptr endpoint;
    bool isOnline;
};


typedef boost::shared_ptr<NMEAMidpoint> NMEAMidpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__NMEAMidpoint__) */
