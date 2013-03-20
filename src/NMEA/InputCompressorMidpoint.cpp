//
//  InputCompressorMidpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 19.03.13.
//
//

#include "InputCompressorMidpoint.h"

boost::shared_ptr<InputCompressorMidpoint> InputCompressorMidpoint::factory(boost::shared_ptr<Endpoint> connectedTo) {
    InputCompressorMidpoint_ptr newMidpoint(new InputCompressorMidpoint(connectedTo));
    newMidpoint->initialize();
    return newMidpoint;
}

void InputCompressorMidpoint::receive_impl(NMEAmsg_ptr msg){
    std::size_t hash = msg->hash_value();
    if(hashes.find(msg->getId())!=hashes.end()){
        if((*(hashes.find(msg->getId()))).second == hash){
            std::cerr << "compressed" << std::endl;
            return;
        }
    }
    hashes.erase(msg->getId());
    hashes.insert(std::pair<std::string, std::size_t>(msg->getId(),hash));
    NMEAEndpoint_ptr nmeaEndpoint = boost::dynamic_pointer_cast<NMEAEndpoint>(getConnectedTo());
    if(nmeaEndpoint){
        nmeaEndpoint->receive(msg);
    }
    else{
        std::cerr << getId()<<": Cannot send Message to none NMEAEndpoint" << std::endl;
    }
}