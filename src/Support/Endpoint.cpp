//
//  Endpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 14.03.13.
//
//

#include "Endpoint.h"
#include "../NMEA/NMEAServer.h"

Endpoint::Endpoint(boost::shared_ptr<Endpoint> connectedTo){
    setConnectedTo(connectedTo);
}

void Endpoint::registerEndpoint(){
    NMEAServer::getInstance()->addEndpoint(this->v_shared_from_this());
    NMEAServer::getInstance()->endpointOnline(this->v_shared_from_this());
}

void Endpoint::unregisterEndpoint(){
    NMEAServer::getInstance()->endpointOffline(this->v_shared_from_this());
    NMEAServer::getInstance()->removeEndpoint(this->v_shared_from_this());
}