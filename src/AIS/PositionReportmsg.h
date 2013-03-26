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
    PositionReportmsg();
    virtual void code(std::string &code);
public:
    virtual ~PositionReportmsg(){}
    
    void setStatus(AISmsg::NavigationStatus status){this->status=status;}
    AISmsg::NavigationStatus getStatus(){return status;}
    
    void setRot(char rot){this->rot=rot;}
    char getRot(){return rot;}
    
    void setSog(unsigned int sog){this->sog=sog;}
    unsigned int getSog(){return sog;}
    
    void setAccuracy(bool accuracy){this->accuracy=accuracy;}
    bool getAccuracy(){return accuracy;}
    
    void setLon(int lon){this->lon=lon;}
    int getLon(){return lon;}
    
    void setLat(int lat){this->lat=lat;}
    int getLat(){return lat;}
    
private:
    AISmsg::NavigationStatus status;
    char rot;
    unsigned int sog;
    bool accuracy;
    int lon;
    int lat;
    unsigned int course;
    unsigned int heading;
    unsigned char timestamp;
    AISmsg::ManeuverIndicator maneuver;
    bool raim;
    unsigned int radio;
};

typedef boost::shared_ptr<PositionReportmsg> PositionReportmsg_ptr;

#endif /* defined(__NMEA_Protocol_Server__PositionReportmsg__) */
