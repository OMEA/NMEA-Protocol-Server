//
//  TimeEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 10.05.13.
//
//

#include <boost/bind.hpp>

#include "TimeEndpoint.h"
#include "Messages/RMCmsg.h"
#include "Messages/ZDAmsg.h"

boost::shared_ptr<TimeEndpoint> TimeEndpoint::factory(boost::shared_ptr<Endpoint> connectedTo) {
    TimeEndpoint_ptr timeEndpoint(new TimeEndpoint(connectedTo));
    timeEndpoint->initialize();
    return timeEndpoint;
}

TimeEndpoint::TimeEndpoint(boost::shared_ptr<Endpoint> connectedTo): NMEAEndpoint(connectedTo), zone(boost::local_time::posix_time_zone("UTC+0")){
}

void TimeEndpoint::initialize(){
    NMEAEndpoint::initialize();
    unregisterCmd("output");
    unregisterCmd("mirror");
    unregisterCmd("checksum");
    unregisterCmd("incompress");
    registerIntCmd("incompress","Input Compressor Number", "Defines the maximum number of consecutive incoming messages that are compressed. 0 = no compresson; -1 = infinite compression", &incompress_messages, -1, -1, std::numeric_limits<int>::max(), false);
    unregisterCmd("outcompress");
    unregisterCmd("out_black");
    unregisterCmd("out_white");
    boost::function<void (Command_ptr)> func = boost::bind(&TimeEndpoint::list_cmd, this, _1);
    registerVoidCmd("list","List of Properties", "List of all properties of the Time receiver.",  func);
    func = boost::bind(&TimeEndpoint::set_zone_cmd, this, _1);
    registerVoidCmd("set_zone","Set Timezone", "Set the timezone the device currently operates in. Default is \"UTC+0\"",  func);
    func = boost::bind(&TimeEndpoint::get_zone_cmd, this, _1);
    registerVoidCmd("get_zone","Get Timezone", "Prints the configured timezone the device currently operates in.",  func);
    receivedData=false;
    registerEndpoint();
}

void TimeEndpoint::list_cmd(Command_ptr command){
    std::ostringstream oss;
    if(receivedData){
        oss << "lala:" << std::endl;
        command->answer(oss.str(), v_shared_from_this());
    }
}

void TimeEndpoint::set_zone_cmd(Command_ptr command){
    try{
        setTimezone(boost::local_time::posix_time_zone(command->getArguments()));
        command->answer("timezone successfully changed to "+command->getArguments()+"\n", this->shared_from_this());
    }
    catch(std::exception e){
        command->answer(Answer::UNKNOWN_CMD, "Problem parsing "+command->getArguments()+": "+e.what()+"\n", this->shared_from_this());
    }
}

void TimeEndpoint::get_zone_cmd(Command_ptr command){
    command->answer("timezone is "+getTimezone().to_posix_string()+"\n", this->shared_from_this());
}


void TimeEndpoint::deliver_impl(NMEAmsg_ptr msg){
    if(RMCmsg_ptr rmc = boost::dynamic_pointer_cast<RMCmsg>(msg)){
        time = rmc->getTime();
        
        ZDAmsg_ptr msg(new ZDAmsg(this->v_shared_from_this()));
        msg->setTimeDate(time);
        msg->setTimezone(getTimezone());
        receive(msg);
        
        receivedData=true;
    }
}