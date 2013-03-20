//
//  NMEAMidpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 19.03.13.
//
//

#include "NMEAMidpoint.h"
#include "InputCompressorMidpoint.h"

boost::shared_ptr<NMEAMidpoint> NMEAMidpoint::factory(boost::shared_ptr<Endpoint> connectedTo, std::string midpointClass) {

    if(midpointClass=="incompressor"){
        return InputCompressorMidpoint::factory(connectedTo);
    }
    throw std::invalid_argument("No such Midpoint!");
}

NMEAMidpoint::NMEAMidpoint(boost::shared_ptr<Endpoint> connectedTo): NMEAEndpoint(connectedTo){}
NMEAMidpoint::NMEAMidpoint(): NMEAEndpoint(){}

void NMEAMidpoint::initialize(){
    NMEAEndpoint::initialize();
    isOnline=false;
}

void NMEAMidpoint::addEndpoint(Endpoint_ptr endpoint){
    isOnline=false;
    this->endpoint = endpoint;
}
void NMEAMidpoint::removeEndpoint(Endpoint_ptr endpoint){
    isOnline=false;
    this->endpoint = Endpoint_ptr();
}

void NMEAMidpoint::endpointOnline(Endpoint_ptr endpoint){
    this->endpoint = endpoint;
    isOnline=true;
    registerEndpoint();
}
void NMEAMidpoint::endpointOffline(Endpoint_ptr endpoint){
    this->endpoint = endpoint;
    isOnline=false;
    unregisterEndpoint();
}

void NMEAMidpoint::deliver(Answer_ptr answer){
    CommandEndpoint_ptr cmdEndpoint = boost::dynamic_pointer_cast<CommandEndpoint>(endpoint);
    if(cmdEndpoint){
        cmdEndpoint->deliver(answer);
    }
    else{
        std::cerr << getId()<<":NMEAMidpoint::deliver Cannot send Answer to none CommandEndpoint" << std::endl;
    }
}

void NMEAMidpoint::receive(Command_ptr command){
    CommandEndpoint_ptr cmdEndpoint = boost::dynamic_pointer_cast<CommandEndpoint>(getConnectedTo());
    if(cmdEndpoint){
        cmdEndpoint->deliver(command);
    }
    else{
        std::cerr << getId()<<": Cannot send Command to none CommandEndpoint" << std::endl;
    }
}

void NMEAMidpoint::deliver_impl(NMEAmsg_ptr msg){
    NMEAEndpoint_ptr nmeaEndpoint = boost::dynamic_pointer_cast<NMEAEndpoint>(endpoint);
    if(nmeaEndpoint){
        nmeaEndpoint->deliver(msg);
    }
    else{
        std::cerr << getId()<<": Cannot send Message to none NMEAEndpoint" << std::endl;
        }
}
void NMEAMidpoint::receive_impl(NMEAmsg_ptr msg){
    NMEAEndpoint_ptr nmeaEndpoint = boost::dynamic_pointer_cast<NMEAEndpoint>(getConnectedTo());
    if(nmeaEndpoint){
        nmeaEndpoint->receive(msg);
    }
    else{
        std::cerr << getId()<<": Cannot send Message to none NMEAEndpoint" << std::endl;
    }
}

std::string NMEAMidpoint::getId(){
    if(endpoint){
        return "NMEAMidpoint|"+endpoint->getId();
    }
    else{
        return "NMEAMidpoint";
    }
}
