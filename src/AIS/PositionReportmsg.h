//
//  PositionReportmsg.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#ifndef __NMEA_Protocol_Server__PositionReportmsg__
#define __NMEA_Protocol_Server__PositionReportmsg__

#include <iostream>
#include "AISmsg.h"

class PositionReportmsg: public AISmsg{
public:
    static boost::shared_ptr<PositionReportmsg> factory(std::string parseMsg);
protected:
    PositionReportmsg(std::string parseMsg);
public:
    virtual ~PositionReportmsg(){}
    void setLon(int lon){this->lon=lon;}
    int getLon(){return lon;}
    void setLat(int lat){this->lat=lat;}
    int getLat(){return lat;}
private:
    int lon;
    int lat;
};

typedef boost::shared_ptr<PositionReportmsg> PositionReportmsg_ptr;

#endif /* defined(__NMEA_Protocol_Server__PositionReportmsg__) */
