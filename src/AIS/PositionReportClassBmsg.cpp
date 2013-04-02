//
//  PositionReportClassBmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#include "PositionReportClassBmsg.h"

boost::shared_ptr<PositionReportClassBmsg> PositionReportClassBmsg::factory(std::string parseMsg) {
    boost::shared_ptr<PositionReportClassBmsg> ret(new PositionReportClassBmsg(parseMsg));
    return ret;
}

PositionReportClassBmsg::PositionReportClassBmsg(unsigned int mmsi): AISmsg(mmsi){
    setBitLength(168);
    setType(AISmsg::STANDARD_CLASS_B_CS_POSITION_REPORT);
    sog = 1023;
    accuracy = false;
    lon = 0x6791AC0;
    lat = 0x3412140;
    course = 0xE10;
    heading = 511;
    second = 60;
    cs = false; //CHECK
    display = false;
    dsc = false;
    band = false;
    msg22 = false;
    assigned = false;
    raim = false;
    radio = 0; //CHECK!
}


PositionReportClassBmsg::PositionReportClassBmsg(std::string parseMsg): AISmsg(parseMsg){
    setBitLength(168);
    sog = getUBits(parseMsg, 46, 10);
    accuracy = getUBits(parseMsg, 56, 1);
    lon = getSBits(parseMsg, 57, 28);
    lat = getSBits(parseMsg, 85, 27);
    course = getUBits(parseMsg, 112, 12);
    heading = getUBits(parseMsg, 124, 9);
    second = getUBits(parseMsg, 133, 6);
    cs = getUBits(parseMsg, 141, 1);
    display = getUBits(parseMsg, 142, 1);
    dsc = getUBits(parseMsg, 143, 1);
    band = getUBits(parseMsg, 144, 1);
    msg22 = getUBits(parseMsg, 145, 1);
    assigned = getUBits(parseMsg, 146, 1);
    raim = getUBits(parseMsg, 147, 1);
    radio = getUBits(parseMsg, 148, 20);
}

void PositionReportClassBmsg::code(std::string &code){
    unsigned int length=code.length();
    unsigned int required=(((getBitLength()-1)/8)+1);
    if(length<required){
        code.append(required-length, 0);
    }
    setUBits(code, 46, 10, sog);
    setUBits(code, 56, 1, accuracy);
    setSBits(code, 57, 28, lon);
    setSBits(code, 85, 27, lat);
    setUBits(code, 112, 12, course);
    setUBits(code, 124, 9, heading);
    setUBits(code, 133, 6, second);
    setUBits(code, 141, 1, cs);
    setUBits(code, 142, 1, display);
    setUBits(code, 143, 1, dsc);
    setUBits(code, 144, 1, band);
    setUBits(code, 145, 1, msg22);
    setUBits(code, 146, 1, assigned);
    setUBits(code, 147, 1, raim);
    setUBits(code, 148, 20, radio);
    
    AISmsg::code(code);
}