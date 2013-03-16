//
//  Endpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 14.03.13.
//
//

#ifndef __NMEA_Protocol_Server__Endpoint__
#define __NMEA_Protocol_Server__Endpoint__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class Endpoint
{
public:
    virtual ~Endpoint(){}
    virtual std::string getId()=0;
protected:
    virtual void registerEndpoint();
    virtual void unregisterEndpoint();
    virtual boost::shared_ptr<Endpoint> v_shared_from_this()=0;
};

typedef boost::shared_ptr<Endpoint> Endpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__Endpoint__) */
