//
//  RMCmsg.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#ifndef __NMEA_Protocol_Server__RMCmsg__
#define __NMEA_Protocol_Server__RMCmsg__

#include <iostream>
#include "../NMEAmsg.h"
#include "navobjects.h"

class RMCmsg: public NMEAmsg {
public:
    RMCmsg(std::string parseMsg, Endpoint_ptr sender, bool check_checksum=false);
    void setTime(boost::posix_time::ptime time){this->time=time;}
    const boost::posix_time::ptime getTime()const{return time;}
    void setState(State state){this->state=state;}
    const State getState()const {return state;}
    void setLatitude(Latitude latitude){this->latitude=latitude;}
    const Latitude getLatitude()const {return latitude;}
    void setLongitude(Longitude longitude){this->longitude=longitude;}
    const Longitude getLongitude()const {return longitude;}
    void setSpeed(double speed){this->speed=speed;}
    const double getSpeed()const {return speed;}
    void setCourse(double course){this->course=fmod(course,360);}
    const double getCourse()const {return course;}
    
    virtual void setMsg(std::string msg);
private:
    boost::posix_time::ptime time;
    State state;
    Latitude latitude;
    Longitude longitude;
    double speed;
    double course;
};

typedef boost::shared_ptr<RMCmsg> RMCmsg_ptr;

#endif /* defined(__NMEA_Protocol_Server__RMCmsg__) */
