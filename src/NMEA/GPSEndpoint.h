//
//  GPSEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 20.03.13.
//
//

#ifndef __NMEA_Protocol_Server__GPSEndpoint__
#define __NMEA_Protocol_Server__GPSEndpoint__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "NMEAEndpoint.h"
#include "Messages/navobjects.h"

class GPSEndpoint: public NMEAEndpoint, public boost::enable_shared_from_this<GPSEndpoint>{
public:
    static boost::shared_ptr<GPSEndpoint> factory(boost::shared_ptr<Endpoint> connectedTo, std::string filename=std::string(""));
protected:
    GPSEndpoint(boost::shared_ptr<Endpoint> connectedTo);
    virtual void initialize();
public:
    std::string getId(){return "GPSreceiver";}
protected:
    virtual void deliver_impl(NMEAmsg_ptr msg);
    virtual void deliverAnswer_impl(Answer_ptr answer){};
    virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
    
private:
    void list_cmd(Command_ptr command);
private:
    bool receivedData;
    boost::posix_time::ptime time;
    State state;
    Latitude latitude;
    Longitude longitude;
    double speed;
    double course;
};

typedef boost::shared_ptr<GPSEndpoint> GPSEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__GPSEndpoint__) */
