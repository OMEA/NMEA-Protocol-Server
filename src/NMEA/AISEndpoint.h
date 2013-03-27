//
//  AISEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#ifndef __NMEA_Protocol_Server__AISEndpoint__
#define __NMEA_Protocol_Server__AISEndpoint__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "NMEAEndpoint.h"

class AISEndpoint: public NMEAEndpoint, public boost::enable_shared_from_this<AISEndpoint>{
public:
    static boost::shared_ptr<AISEndpoint> factory(boost::shared_ptr<Endpoint> connectedTo);
protected:
    AISEndpoint(boost::shared_ptr<Endpoint> connectedTo);
    virtual void initialize();

    virtual void deliver_impl(NMEAmsg_ptr msg);
    virtual void deliverAnswer_impl(Answer_ptr answer){};
    
    virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
public:
    std::string getId(){return "AISreceiver";}
private:
    std::string ais_decode(std::string bitstream);
};

typedef boost::shared_ptr<AISEndpoint> AISEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__AISEndpoint__) */
