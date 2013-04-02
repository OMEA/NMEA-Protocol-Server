//
//  PositionReportmsg.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#ifndef __NMEA_Protocol_Server__PositionReportClassBmsg__
#define __NMEA_Protocol_Server__PositionReportClassBmsg__

#include <iostream>
#include "AISmsg.h"

class PositionReportClassBmsg: public AISmsg{
public:
    static boost::shared_ptr<PositionReportClassBmsg> factory(std::string parseMsg);
    PositionReportClassBmsg(unsigned int mmsi);
protected:
    PositionReportClassBmsg(std::string parseMsg);
    virtual void code(std::string &code);
public:
    virtual ~PositionReportClassBmsg(){}
    
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
    
    void setSecond(unsigned char second){this->second=second;}
    unsigned char getSecond(){return second;}
    
    void setCS(bool cs){this->cs=cs;}
    bool getCS(){return cs;}
    
    void setDisplay(bool display){this->display=display;}
    bool getDisplay(){return display;}
    
    void setDSC(bool dsc){this->dsc=dsc;}
    bool getDSC(){return dsc;}
    
    void setBand(bool band){this->band=band;}
    bool getBand(){return band;}
    
    void setMsg22(bool band){this->band=band;}
    bool getMsg22(){return band;}
    
    void setAssigned(bool assigned){this->assigned=assigned;}
    bool getAssigned(){return assigned;}
    
    void setRaim(bool msg22){this->msg22=msg22;}
    bool getRaim(){return msg22;}
    
    void setRadio(unsigned int radio){this->radio=radio;}
    unsigned int getRadio(){return radio;}
private:
    unsigned int sog;
    bool accuracy;
    int lon;
    int lat;
    unsigned int course;
    unsigned int heading;
    unsigned char second;
    bool cs;
    bool display;
    bool dsc;
    bool band;
    bool msg22;
    bool assigned;
    bool raim;
    unsigned int radio;
};

typedef boost::shared_ptr<PositionReportClassBmsg> PositionReportClassBmsg_ptr;

#endif /* defined(__NMEA_Protocol_Server__PositionReportmsg__) */
