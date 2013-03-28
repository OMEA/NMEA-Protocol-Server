//
//  StaticVoyageRelatedmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 26.03.13.
//
//

#include "StaticVoyageRelatedmsg.h"

boost::shared_ptr<StaticVoyageRelatedmsg> StaticVoyageRelatedmsg::factory(std::string parseMsg) {
    boost::shared_ptr<StaticVoyageRelatedmsg> ret(new StaticVoyageRelatedmsg(parseMsg));
    return ret;
}

StaticVoyageRelatedmsg::StaticVoyageRelatedmsg(unsigned int mmsi): AISmsg(mmsi){
    setBitLength(424);
    ais_version = 0;
    imo = 0;
    callsign = "";
    shipname = "";
    shiptype = AISmsg::SHIP_TYPE_NOT_AVAILABLE;
    to_bow = 0;
    to_stern = 0;
    to_port = 0;
    to_starboard = 0;
    epfd = AISmsg::POSITION_FIX_UNDEFINED;
    month = 0;
    day = 0;
    hour = 24;
    minute = 60;
    draught = 0;
    destination = "";
    dte = false;
}


StaticVoyageRelatedmsg::StaticVoyageRelatedmsg(std::string parseMsg): AISmsg(parseMsg){
    setBitLength(424);
    ais_version = getUBits(parseMsg, 38, 2);
    imo = getUBits(parseMsg, 40, 30);
    callsign = getStringBits(parseMsg, 70, 42);
    shipname = getStringBits(parseMsg, 112, 120);
    setShiptype((AISmsg::ShipType)getUBits(parseMsg, 232, 8));
    to_bow = getUBits(parseMsg, 240, 9);
    to_stern = getUBits(parseMsg, 249, 9);
    to_port = getUBits(parseMsg, 258, 6);
    to_starboard = getUBits(parseMsg, 264, 6);
    setEPFD((AISmsg::PositionFixType)getUBits(parseMsg, 270, 4));
    month = getUBits(parseMsg, 274, 4);
    day = getUBits(parseMsg, 278, 5);
    hour = getUBits(parseMsg, 283, 5);
    minute = getUBits(parseMsg, 288, 6);
    draught = getUBits(parseMsg, 294, 8);
    destination = getStringBits(parseMsg, 302, 120);
    dte = getUBits(parseMsg, 422, 1);
}

void StaticVoyageRelatedmsg::code(std::string &code){
    unsigned int length=code.length();
    unsigned int required=(((getBitLength()-1)/8)+1);
    if(length<required){
        code.append(required-length, 0);
    }
    setUBits(code, 38, 2, ais_version);
    setUBits(code, 40, 30, imo);
    setStringBits(code, 70, 42, callsign);
    setStringBits(code, 112, 120, shipname);
    setUBits(code, 232, 8, shiptype);
    setUBits(code, 240, 9, to_bow);
    setUBits(code, 249, 9, to_stern);
    setUBits(code, 258, 6, to_port);
    setUBits(code, 264, 6, to_starboard);
    setUBits(code, 270, 4, epfd);
    setUBits(code, 274, 4, month);
    setUBits(code, 278, 5, day);
    setUBits(code, 283, 5, hour);
    setUBits(code, 288, 6, minute);
    setUBits(code, 294, 8, draught);
    setStringBits(code, 302, 120, destination);
    setUBits(code, 422, 1, dte);
    AISmsg::code(code);
}