//
//  AISEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#include "AISEndpoint.h"
#include "../NMEA/Messages/navobjects.h"
#include "../NMEA/Messages/AIVDmsg.h"
#include "../AIS/AISmsg.h"
#include "../AIS/PositionReportmsg.h"
#include "../AIS/StaticVoyageRelatedmsg.h"

boost::shared_ptr<AISEndpoint> AISEndpoint::factory(boost::shared_ptr<Endpoint> connectedTo) {
    AISEndpoint_ptr aisEndpoint(new AISEndpoint(connectedTo));
    aisEndpoint->initialize();
    return aisEndpoint;
}


AISEndpoint::AISEndpoint(boost::shared_ptr<Endpoint> connectedTo): NMEAEndpoint(connectedTo){
    std::string lala = "177KQJ5000G?tO`K>RA1wUbN0TKH";
    lala = "139OpT5P?wdtSF0l4Q@>4?wp2@CR";
    lala = "176qwD00000eUfrNVNn=EP`P0896";
    lala = "537aeL02>mUP?ADc800TADp4000000000000001601s0F4a>N@j0C@UDQh0000000000000";
    std::string dede = ais_decode(lala);
    std::cout << lala << "->" << dede << std::endl;
}

void AISEndpoint::initialize(){
    NMEAEndpoint::initialize();
    unregisterCmd("output");
    unregisterCmd("mirror");
    unregisterCmd("checksum");
    unregisterCmd("incompress");
    registerIntCmd("incompress","Input Compressor Number", "Defines the maximum number of consecutive incoming messages that are compressed. 0 = no compresson; -1 = infinite compression", &incompress_messages, -1, -1, std::numeric_limits<int>::max(), false);
    unregisterCmd("outcompress");
    unregisterCmd("out_black");
    unregisterCmd("out_white");
    //boost::function<void (Command_ptr)> func = boost::bind(&GPSEndpoint::list_cmd, this, _1);
    //registerVoidCmd("list","List of Properties", "List of all properties of the GPS receiver.",  func);
    registerEndpoint();
}

std::string AISEndpoint::ais_decode(std::string bitstream){
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
        oss << "speed:" << (double)rep->getSog()/10 << std::endl;
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
        oss << "course:" << (double)rep->getCourse()/10 << std::endl;
        oss << "heading:" << rep->getHeading() << std::endl;
        oss << "second:" << (unsigned int)rep->getTimestamp() << std::endl;
        oss << "maneuver:" << rep->getManeuver() << std::endl;
        oss << "raim:" << rep->getRaim() << std::endl;
        oss << "radio:" << rep->getRadio() << std::endl;
    }
    StaticVoyageRelatedmsg_ptr stat = boost::dynamic_pointer_cast<StaticVoyageRelatedmsg>(msg);
    if(stat){
        oss << "callsign:" << stat->getCallsign() << std::endl;
        oss << "shipname:" << stat->getShipname() << std::endl;
        oss << "shiptype:" << stat->getShiptype() << std::endl;
        oss << "toBow:" << stat->getToBow() << std::endl;
        oss << "toStern:" << stat->getToStern() << std::endl;
        oss << "toPort:" << (unsigned int)stat->getToPort() << std::endl;
        oss << "toStarboard:" << (unsigned int)stat->getToStarboard() << std::endl;
        oss << "draught:" << (float)stat->getDraught()/10 << std::endl;
        oss << "epfd:" << (unsigned int)stat->getEPFD() << std::endl;
        oss << "ETA:" << (unsigned int)stat->getDay() << "." << (unsigned int)stat->getMonth() << " " << (unsigned int)stat->getHour() << ":" << (unsigned int)stat->getMinute() << std::endl;
        oss << "destination:" << stat->getDestination() << std::endl;
        oss << "dte:" << (unsigned int)stat->getDTE() << std::endl;
        
    }
    
    return oss.str();
}

void AISEndpoint::deliver_impl(NMEAmsg_ptr msg){
    if(AIVDmsg_ptr aivd = boost::dynamic_pointer_cast<AIVDmsg>(msg)){
        std::cout << "AIS-MESSAGE" << std::endl;
        std::cout << ais_decode(aivd->getPayload()) << std::endl;
    }
}