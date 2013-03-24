//
//  AISEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#include "AISEndpoint.h"
#include "../AIS/AISmsg.h"
#include "../AIS/PositionReportmsg.h"

boost::shared_ptr<AISEndpoint> AISEndpoint::factory(boost::shared_ptr<Endpoint> connectedTo) {
    AISEndpoint_ptr aisEndpoint(new AISEndpoint(connectedTo));
    aisEndpoint->initialize();
    return aisEndpoint;
}

AISEndpoint::AISEndpoint(boost::shared_ptr<Endpoint> connectedTo): GPSEndpoint(connectedTo){
    std::string lala = "177KQJ5000G?tO`K>RA1wUbN0TKH";
    std::string dede = ais_decode(lala);
    std::cout << lala << "->" << dede << std::endl;
}

std::string AISEndpoint::ais_decode(std::string bitstream){
    //const char sixchr[] = "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^- !\"#$%&'()*+,-./0123456789:;<=>?";
    std::ostringstream oss;
    
    AISmsg_ptr msg = AISmsg::factory(bitstream);
    
    oss << "type:" << msg->getType() << std::endl;
    oss << "repeat:" << (unsigned int)msg->getRepeat() << std::endl;
    oss << "mmsi:" << msg->getMMSI() << std::endl;
    
    PositionReportmsg_ptr rep = boost::dynamic_pointer_cast<PositionReportmsg>(msg);
    if(rep){
        oss << "lon:" << rep->getLon() << std::endl;
        oss << "lat:" << rep->getLat() << std::endl;
    }
    
    return oss.str();
}