//
//  AISEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#include "AISEndpoint.h"
#include "../NMEA/Messages/navobjects.h"
#include "../AIS/AISmsg.h"
#include "../AIS/PositionReportmsg.h"
#include "../AIS/StaticVoyageRelatedmsg.h"

boost::shared_ptr<AISEndpoint> AISEndpoint::factory(boost::shared_ptr<Endpoint> connectedTo) {
    AISEndpoint_ptr aisEndpoint(new AISEndpoint(connectedTo));
    aisEndpoint->initialize();
    return aisEndpoint;
}


AISEndpoint::AISEndpoint(boost::shared_ptr<Endpoint> connectedTo): GPSEndpoint(connectedTo){
    std::string lala = "177KQJ5000G?tO`K>RA1wUbN0TKH";
    lala = "139OpT5P?wdtSF0l4Q@>4?wp2@CR";
    lala = "176qwD00000eUfrNVNn=EP`P0896";
    lala = "537aeL02>mUP?ADc800TADp4000000000000001601s0F4a>N@j0C@UDQh0000000000000";
    std::string dede = ais_decode(lala);
    std::cout << lala << "->" << dede << std::endl;
}

std::string AISEndpoint::ais_decode(std::string bitstream){
    //const char sixchr[] = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^- !\"#$%&'()*+,-./0123456789:;<=>?";
    std::ostringstream oss;
    
    AISmsg_ptr msg = AISmsg::factory(bitstream);
    
    std::cout << bitstream << " len: " << bitstream.length() << std::endl;
    std::string coded = msg->toCodedStr();
    std::cout << coded << " len: " << coded.length() << std::endl;
    
    oss << "type:" << msg->getType() << std::endl;
    oss << "repeat:" << (unsigned int)msg->getRepeat() << std::endl;
    oss << "mmsi:" << msg->getMMSI() << std::endl;
    
    PositionReportmsg_ptr rep = boost::dynamic_pointer_cast<PositionReportmsg>(msg);
    if(rep){
        if(rep->getLon()!=0x6791AC0){
            oss << "lon:" << Longitude((double)rep->getLon()/10000).to_str() << std::endl;
        }else{
            oss << "lon: INVALID" << std::endl;
        }
        if(rep->getLat()!=0x3412140){
            oss << "lat:" << Latitude((double)rep->getLat()/10000).to_str() << std::endl;
        }else{
            oss << "lat: INVALID" << std::endl;
        }
        
    }
    StaticVoyageRelatedmsg_ptr stat = boost::dynamic_pointer_cast<StaticVoyageRelatedmsg>(msg);
    if(stat){
        oss << "callsign:" << stat->getCallsign() << std::endl;
        oss << "shipname:" << stat->getShipname() << std::endl;
    }
    
    return oss.str();
}