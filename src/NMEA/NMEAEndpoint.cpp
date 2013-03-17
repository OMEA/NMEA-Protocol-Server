//
//  NMEAEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "NMEAEndpoint.h"

NMEAEndpoint::NMEAEndpoint(boost::shared_ptr<Endpoint> connectedTo): CommandEndpoint(connectedTo){
    registerBoolCmd("input", &input, true, true);
    registerBoolCmd("output", &output, true, true);
    registerBoolCmd("mirror", &portmirror, false, true);
    registerBoolCmd("checksum", &checksum, true, true);
}

NMEAEndpoint::NMEAEndpoint(){
    registerBoolCmd("input", &input, true, true);
    registerBoolCmd("output", &output, true, true);
    registerBoolCmd("mirror", &portmirror, false, true);
    registerBoolCmd("checksum", &checksum, true, true);
}

void NMEAEndpoint::deliver(NMEAmsg_ptr msg){
    if(output && (portmirror || msg->getSender()!=this->v_shared_from_this()))
        deliver_impl(msg);
}

void NMEAEndpoint::receive(NMEAmsg_ptr msg){
    if(input){
        receive_impl(msg);
    }
}

void NMEAEndpoint::receive_impl(NMEAmsg_ptr msg){
    msg->setSender(this->v_shared_from_this());
    NMEAEndpoint_ptr nmeaEndpoint = boost::dynamic_pointer_cast<NMEAEndpoint>(getConnectedTo());
    if(nmeaEndpoint){
        nmeaEndpoint->receive(msg);
    }
    else{
        std::cerr << "Cannot send NMEA to none NMEAEndpoint" << std::endl;
    }
}

void NMEAEndpoint::receive(Command_ptr command){
    //TODO Helpermethoden damit das nicht ewig lang wird hier!
    if(command->getCommand()=="id"){
        command->answer(getId()+"\n", this->v_shared_from_this());
    }
    else{
        CommandEndpoint::receive(command);
    }
    //TODO: response generieren und bei endpoint delivern
}