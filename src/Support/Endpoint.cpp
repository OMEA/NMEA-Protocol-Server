//
//  Endpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 14.03.13.
//
//

#include "../NMEA/NMEAMidpoint.h"
#include "../NMEA/NMEAServer.h"
#include <boost/shared_ptr.hpp>

Endpoint::Endpoint(boost::shared_ptr<Endpoint> connectedTo){
    setConnectedTo(connectedTo);
}

void Endpoint::registerEndpoint(){
    NMEAMidpoint_ptr nmeaMidpoint = boost::dynamic_pointer_cast<NMEAMidpoint>(connectedTo);
    if(nmeaMidpoint){
        nmeaMidpoint->addEndpoint(this->v_shared_from_this());
        nmeaMidpoint->endpointOnline(this->v_shared_from_this());

    }
    else {
        std::cerr << "Endpoint is not connected to a NMEAMidpoint" << std::endl;
    }
}

void Endpoint::unregisterEndpoint(){
    NMEAMidpoint_ptr nmeaMidpoint = boost::dynamic_pointer_cast<NMEAMidpoint>(connectedTo);
    if(nmeaMidpoint){
        nmeaMidpoint->endpointOffline(this->v_shared_from_this());
        nmeaMidpoint->removeEndpoint(this->v_shared_from_this());
        
    }
    else{
        std::cerr << "Endpoint is not connected to a NMEAMidpoint" << std::endl;
    }
}