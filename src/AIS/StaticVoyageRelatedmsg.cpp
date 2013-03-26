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

StaticVoyageRelatedmsg::StaticVoyageRelatedmsg(): AISmsg(){
    setBitLength(424);
    ais_version = 0;
    imo = 0;
    callsign = "";
    shipname = "";
}


StaticVoyageRelatedmsg::StaticVoyageRelatedmsg(std::string parseMsg): AISmsg(parseMsg){
    setBitLength(424);
    ais_version = getUBits(parseMsg, 38, 2);
    imo = getUBits(parseMsg, 40, 30);
    callsign = getStringBits(parseMsg, 70, 42);
    shipname = getStringBits(parseMsg, 112, 120);
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

    
    AISmsg::code(code);
}