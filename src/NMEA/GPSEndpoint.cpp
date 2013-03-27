//
//  GPSEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 20.03.13.
//
//

#include <boost/bind.hpp>
#include "GPSEndpoint.h"
#include "Messages/RMCmsg.h"

boost::shared_ptr<GPSEndpoint> GPSEndpoint::factory(boost::shared_ptr<Endpoint> connectedTo) {
    GPSEndpoint_ptr gpsEndpoint(new GPSEndpoint(connectedTo));
    gpsEndpoint->initialize();
    return gpsEndpoint;
}

GPSEndpoint::GPSEndpoint(boost::shared_ptr<Endpoint> connectedTo): NMEAEndpoint(connectedTo){
}

void GPSEndpoint::initialize(){
    NMEAEndpoint::initialize();
    unregisterCmd("output");
    unregisterCmd("mirror");
    unregisterCmd("checksum");
    unregisterCmd("incompress");
    registerIntCmd("incompress","Input Compressor Number", "Defines the maximum number of consecutive incoming messages that are compressed. 0 = no compresson; -1 = infinite compression", &incompress_messages, -1, -1, std::numeric_limits<int>::max(), false);
    unregisterCmd("outcompress");
    unregisterCmd("out_black");
    unregisterCmd("out_white");
    boost::function<void (Command_ptr)> func = boost::bind(&GPSEndpoint::list_cmd, this, _1);
    registerVoidCmd("list","List of Properties", "List of all properties of the GPS receiver.",  func);
    receivedData=false;
    registerEndpoint();
}

void GPSEndpoint::list_cmd(Command_ptr command){
    std::ostringstream oss;
    if(receivedData){
        oss << "Data:" << std::endl;
        oss << "time:" << "\t\t" << to_simple_string(time) << std::endl;
        oss << "state:" << "\t\t" << ((state==ACTIVE)?"valid":"invalid") << std::endl;
        oss << "position:" << "\t" << latitude.getDegrees() << "°" << latitude.getMinutes() << "\"" << ' ' << ((latitude.getSign()==Latitude::NORTH)?"N":"S") << std::endl;
        oss << "\t\t"<< longitude.getDegrees() << "°" << longitude.getMinutes() << "\"" << ' ' << ((longitude.getSign()==Longitude::EAST)?"E":"W") << std::endl;
        oss << "speed:" << "\t\t" << speed << " knots" << std::endl;
        oss << "course:" << "\t\t" << course << "°" << std::endl;
        command->answer(oss.str(), v_shared_from_this());
    }
}


void GPSEndpoint::deliver_impl(NMEAmsg_ptr msg){
    if(RMCmsg_ptr rmc = boost::dynamic_pointer_cast<RMCmsg>(msg)){
        time = rmc->getTime();
        state = rmc->getState();
        latitude = rmc->getLatitude();
        longitude = rmc->getLongitude();
        speed = rmc->getSpeed();
        course = rmc->getCourse();
        receivedData=true;
    }
}