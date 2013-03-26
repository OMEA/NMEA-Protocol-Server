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
protected:
    StaticVoyageRelatedmsg(std::string parseMsg);
    StaticVoyageRelatedmsg();
    virtual void code(std::string &code);
public:
    virtual ~StaticVoyageRelatedmsg(){}
    
    void setAis_Version(unsigned char ais_version){this->ais_version=ais_version;}
    unsigned char getAis_Version(){return ais_version;}
    
    void setImo(unsigned int imo){this->imo=imo;}
    unsigned int getImo(){return imo;}
    
    void setCallsign(unsigned int callsign){this->callsign=callsign;}
    std::string getCallsign(){return callsign;}
    
    void setShipname(unsigned int shipname){this->shipname=shipname;}
    std::string getShipname(){return shipname;}
    
private:
    unsigned char ais_version;
    unsigned int imo;
    std::string callsign;
    std::string shipname;
};

typedef boost::shared_ptr<StaticVoyageRelatedmsg> StaticVoyageRelatedmsg_ptr;

#endif /* defined(__NMEA_Protocol_Server__StaticVoyageRelatedmsg__) */
