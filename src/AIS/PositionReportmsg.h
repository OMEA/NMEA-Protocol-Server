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
    PositionReportmsg();
protected:
    PositionReportmsg(std::string parseMsg);
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
    
    void setCourse(unsigned int course){this->course=course;}
    unsigned int getCourse(){return course;}
    
    void setHeading(unsigned int heading){this->heading=heading;}
    unsigned int getHeading(){return heading;}
    
    void setTimestamp(unsigned char timestamp){this->timestamp=timestamp;}
    unsigned char getTimestamp(){return timestamp;}
    
    void setManeuver(AISmsg::ManeuverIndicator maneuver){this->maneuver=maneuver;}
    AISmsg::ManeuverIndicator getManeuver(){return maneuver;}
    
    void setRaim(bool raim){this->raim=raim;}
    bool getRaim(){return raim;}
    
    void setRadio(unsigned int radio){this->radio=radio;}
    unsigned int getRadio(){return radio;}
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
