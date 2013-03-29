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
protected:
    Endpoint(){connectedTo=boost::shared_ptr<Endpoint>();}
    Endpoint(boost::shared_ptr<Endpoint> connectedTo);
    virtual void initialize();
public:
    virtual ~Endpoint(){}
    virtual std::string getId()=0;
    
protected:
    virtual void registerEndpoint();
    virtual void unregisterEndpoint();
    virtual boost::shared_ptr<Endpoint> v_shared_from_this()=0;
    void setConnectedTo(boost::shared_ptr<Endpoint> connectedTo){this->connectedTo=connectedTo;}
    boost::shared_ptr<Endpoint> getConnectedTo(){return connectedTo;}
private:
    boost::shared_ptr<Endpoint> connectedTo;
};

typedef boost::shared_ptr<Endpoint> Endpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__Endpoint__) */
