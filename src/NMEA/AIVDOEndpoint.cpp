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

boost::shared_ptr<AIVDOEndpoint> AIVDOEndpoint::factory(boost::shared_ptr<Endpoint> connectedTo, unsigned int mmsi) {
    AIVDOEndpoint_ptr aivdoEndpoint(new AIVDOEndpoint(connectedTo));
    aivdoEndpoint->setMMSI(mmsi);
    aivdoEndpoint->initialize();
    return aivdoEndpoint;
}


AIVDOEndpoint::AIVDOEndpoint(boost::shared_ptr<Endpoint> connectedTo): GPSEndpoint(connectedTo){}

void AIVDOEndpoint::initialize(){
    GPSEndpoint::initialize();
    unregisterCmd("incompress");
    registerIntCmd("incompress","Input Compressor Number", "Defines the maximum number of consecutive incoming messages that are compressed. 0 = no compresson; -1 = infinite compression", &incompress_messages, 0, -1, std::numeric_limits<int>::max(), false);
    //boost::function<void (Command_ptr)> func = boost::bind(&GPSEndpoint::list_cmd, this, _1);
    //registerVoidCmd("list","List of Properties", "List of all properties of the GPS receiver.",  func);
}

std::string AIVDOEndpoint::getId(){
    std::ostringstream oss;
    oss << "virtualAIVDO:" << getMMSI();
    return oss.str();
}

void AIVDOEndpoint::sendPositionReportClassA(){
    PositionReportmsg_ptr pr = boost::shared_ptr<PositionReportmsg>(new PositionReportmsg(getMMSI()));
    pr->setStatus(AISmsg::MOORED);
    pr->setSog((unsigned int)(getSpeed()*10));
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
    pr->setLat(lat);
    pr->setCourse(((unsigned int)getCourse()*10));
    pr->setHeading(511);
    pr->setTimestamp(getTime().time_of_day().seconds());
    pr->setManeuver(AISmsg::NOT_AVAILABLE);
    pr->setRaim(false);
    pr->setRadio(0);
    
    AIVDmsg_ptr msg(new AIVDmsg(this->v_shared_from_this(), true));
    msg->setFragmentCount(1);
    msg->setFragment(1);
    msg->setMessageId(0);
    msg->setChannelCode('B');
    std::string payload = pr->toCodedStr();
    msg->setPayload(payload);
    msg->setFillBits(pr->getBitLength()%6);
    receive(msg);
    
    AIVDmsg_ptr msg2(new AIVDmsg(this->v_shared_from_this(), false));
    msg2->setFragmentCount(1);
    msg2->setFragment(1);
    msg2->setMessageId(0);
    msg2->setChannelCode('B');
    std::string payload2 = pr->toCodedStr();
    msg2->setPayload(payload2);
    msg2->setFillBits(pr->getBitLength()%6);
    receive(msg2);
}

void AIVDOEndpoint::deliver_impl(NMEAmsg_ptr msg){
    GPSEndpoint::deliver_impl(msg);
    if(RMCmsg_ptr rmc = boost::dynamic_pointer_cast<RMCmsg>(msg)){
        sendPositionReportClassA();
    }
}