//
//  AIVDOEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 27.03.13.
//
//

#include "AIVDOEndpoint.h"
#include "Messages/AIVDmsg.h"
#include "Messages/RMCmsg.h"
#include "../AIS/PositionReportmsg.h"

boost::shared_ptr<AIVDOEndpoint> AIVDOEndpoint::factory(boost::shared_ptr<Endpoint> connectedTo) {
    AIVDOEndpoint_ptr aivdoEndpoint(new AIVDOEndpoint(connectedTo));
    aivdoEndpoint->initialize();
    return aivdoEndpoint;
}


AIVDOEndpoint::AIVDOEndpoint(boost::shared_ptr<Endpoint> connectedTo): GPSEndpoint(connectedTo){}

void AIVDOEndpoint::initialize(){
    GPSEndpoint::initialize();
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

void AIVDOEndpoint::sendPositionReportClassA(){
    PositionReportmsg_ptr pr = boost::shared_ptr<PositionReportmsg>(new PositionReportmsg());
    pr->setStatus(AISmsg::NOT_DEFINED);
    pr->setSog(((unsigned int)getSpeed()*10));
    pr->setAccuracy(false);
    int lon = 0;
    lon += getLongitude().getDegrees()*600000;
    lon += getLongitude().getMinutes()*10000;
    if(getLongitude().getSign()==Longitude::WEST){
        lon*=-1;
    }
    pr->setLon(lon);
    int lat = 0;
    lat += getLatitude().getDegrees()*600000;
    lat += getLatitude().getMinutes()*10000;
    if(getLatitude().getSign()==Latitude::SOUTH){
        lat*=-1;
    }
    pr->setCourse(((unsigned int)getCourse()*10));
    pr->setHeading(511);
    pr->setTimestamp(getTime().time_of_day().seconds());
    pr->setManeuver(AISmsg::NOT_AVAILABLE);
    pr->setRaim(false);
    pr->setRadio(0);
}

void AIVDOEndpoint::deliver_impl(NMEAmsg_ptr msg){
    GPSEndpoint::deliver_impl(msg);
    if(RMCmsg_ptr rmc = boost::dynamic_pointer_cast<RMCmsg>(msg)){
        sendPositionReportClassA();
    }
}