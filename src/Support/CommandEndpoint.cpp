//
//  CommandEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 14.03.13.
//
//

#include "CommandEndpoint.h"
#include "../NMEA/NMEAServer.h"

void CommandEndpoint::deliver(Command_ptr command){
    if((*command).getReceiver()=="local"){
        receive(command);
    }
    else{
        NMEAServer::getInstance()->receive(command);
    }
}

void CommandEndpoint::deliver(Answer_ptr answer){
    deliverAnswer_impl(answer);
}

void CommandEndpoint::receive(Command_ptr command){
    command->answer("Cannot understand command "+command->getCommand()+"\n", this->v_shared_from_this());
}
