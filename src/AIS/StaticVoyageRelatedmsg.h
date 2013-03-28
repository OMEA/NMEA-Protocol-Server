//
//  StaticVoyageRelatedmsg.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 26.03.13.
//
//

#ifndef __NMEA_Protocol_Server__StaticVoyageRelatedmsg__
#define __NMEA_Protocol_Server__StaticVoyageRelatedmsg__

#include <iostream>
#include "AISmsg.h"

class StaticVoyageRelatedmsg: public AISmsg{
public:
    static boost::shared_ptr<StaticVoyageRelatedmsg> factory(std::string parseMsg);
    StaticVoyageRelatedmsg(unsigned int mmsi);
protected:
    StaticVoyageRelatedmsg(std::string parseMsg);
    virtual void code(std::string &code);
public:
    virtual ~StaticVoyageRelatedmsg(){}
    
    void setAis_Version(unsigned char ais_version){this->ais_version=ais_version;}
    unsigned char getAis_Version(){return ais_version;}
    
    void setImo(unsigned int imo){this->imo=imo;}
    unsigned int getImo(){return imo;}
    
    void setCallsign(std::string callsign){this->callsign=callsign;}
    std::string getCallsign(){return callsign;}
    
    void setShipname(std::string shipname){this->shipname=shipname;}
    std::string getShipname(){return shipname;}
    
    void setShiptype(AISmsg::ShipType shiptype){(shiptype>99)?(this->shiptype=AISmsg::SHIP_TYPE_NOT_AVAILABLE):(this->shiptype=shiptype);}
    AISmsg::ShipType getShiptype(){return shiptype;}
    
    void setToBow(unsigned int to_bow){this->to_bow=to_bow;}
    unsigned int getToBow(){return to_bow;}
    
    void setToStern(unsigned int to_stern){this->to_stern=to_stern;}
    unsigned int getToStern(){return to_stern;}
    
    void setToPort(unsigned char to_port){this->to_port=to_port;}
    unsigned char getToPort(){return to_port;}
    
    void setToStarboard(unsigned char to_starboard){this->to_starboard=to_starboard;}
    unsigned char getToStarboard(){return to_starboard;}
    
    void setEPFD(AISmsg::PositionFixType epfd){(epfd>9)?(this->epfd=AISmsg::POSITION_FIX_UNDEFINED):(this->epfd=epfd);}
    AISmsg::PositionFixType getEPFD(){return epfd;}
    
    void setMonth(unsigned char month){this->month=month;}
    unsigned char getMonth(){return month;}
    
    void setDay(unsigned char day){this->day=day;}
    unsigned char getDay(){return day;}
    
    void setHour(unsigned char hour){this->hour=hour;}
    unsigned char getHour(){return hour;}
    
    void setMinute(unsigned char minute){this->minute=minute;}
    unsigned char getMinute(){return minute;}
    
    void setDraught(unsigned char draught){this->draught=draught;}
    unsigned char getDraught(){return draught;}
    
    void setDestination(std::string destination){this->destination=destination;}
    std::string getDestination(){return destination;}
    
    void setDTE(bool dte){this->dte=dte;}
    bool getDTE(){return dte;}
private:
    unsigned char ais_version;
    unsigned int imo;
    std::string callsign;
    std::string shipname;
    AISmsg::ShipType shiptype;
    unsigned int to_bow;
    unsigned int to_stern;
    unsigned char to_port;
    unsigned char to_starboard;
    AISmsg::PositionFixType epfd;
    unsigned char month;
    unsigned char day;
    unsigned char hour;
    unsigned char minute;
    unsigned char draught;
    std::string destination;
    bool dte;
};

typedef boost::shared_ptr<StaticVoyageRelatedmsg> StaticVoyageRelatedmsg_ptr;

#endif /* defined(__NMEA_Protocol_Server__StaticVoyageRelatedmsg__) */
