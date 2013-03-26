//
//  PositionReportmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#include "PositionReportmsg.h"

boost::shared_ptr<PositionReportmsg> PositionReportmsg::factory(std::string parseMsg) {
    boost::shared_ptr<PositionReportmsg> ret(new PositionReportmsg(parseMsg));
    return ret;
}

PositionReportmsg::PositionReportmsg(): AISmsg(){
    setBitLength(168);
    status = AISmsg::NOT_DEFINED;
    rot = 0x80;
    sog = 1023;
    accuracy = false;
    lon = 0x6791AC0;
    lat = 0x3412140;
    course = 0xE10;
    heading = 511;
    timestamp = 60;
    maneuver = AISmsg::NOT_AVAILABLE;
    raim = false;
    radio = 0; //CHECK!
}


PositionReportmsg::PositionReportmsg(std::string parseMsg): AISmsg(parseMsg){
    setBitLength(168);
    status = (AISmsg::NavigationStatus)getUBits(parseMsg, 38, 4);
    rot = getUBits(parseMsg, 42, 8);
    sog = getUBits(parseMsg, 50, 10);
    accuracy = getUBits(parseMsg, 60, 1);
    lon = getSBits(parseMsg, 61, 28);
    lat = getSBits(parseMsg, 89, 27);
    course = getUBits(parseMsg, 116, 12);
    heading = getUBits(parseMsg, 128, 9);
    timestamp = getUBits(parseMsg, 137, 6);
    maneuver = (AISmsg::ManeuverIndicator)getUBits(parseMsg, 143, 2);
    raim = getUBits(parseMsg, 148, 1);
    radio = getUBits(parseMsg, 149, 19);
    
}

void PositionReportmsg::code(std::string &code){
    unsigned int length=code.length();
    unsigned int required=(((getBitLength()-1)/8)+1);
    if(length<required){
        code.append(required-length, 0);
    }
    setUBits(code, 38, 4, status);
    setUBits(code, 42, 8, rot);
    setUBits(code, 50, 10, sog);
    setUBits(code, 60, 1, accuracy);
    setSBits(code, 61, 28, lon);
    setSBits(code, 89, 27, lat);
    setUBits(code, 116, 12, course);
    setUBits(code, 128, 9, heading);
    setUBits(code, 137, 6, timestamp);
    setUBits(code, 143, 2, maneuver);
    setUBits(code, 148, 1, raim);
    setUBits(code, 149, 19, radio);
    
    AISmsg::code(code);
}