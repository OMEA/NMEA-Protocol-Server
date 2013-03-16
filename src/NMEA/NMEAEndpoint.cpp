//
//  NMEAEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "NMEAEndpoint.h"

#include "NMEAServer.h"

NMEAEndpoint::NMEAEndpoint(){
    input=true;
    output=true;
    portmirror=false;
    checksum=true;
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
    NMEAServer::getInstance()->receive(msg);
}

void NMEAEndpoint::receive(Command_ptr command){
    //TODO Helpermethoden damit das nicht ewig lang wird hier!
    if(command->getCommand()=="id"){
        command->answer(getId()+"\n", this->v_shared_from_this());
    }
    else if(command->getCommand()=="input"){
        if(command->getArguments()=="on"){
            input=true;
            command->answer("input is now turned on\n", this->v_shared_from_this());
        }
        else if(command->getArguments()=="off"){
            input=false;
            command->answer("input is now turned off\n", this->v_shared_from_this());
        }
        else{
            command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n", this->v_shared_from_this());
        }
    }
    else if(command->getCommand()=="output"){
        if(command->getArguments()=="on"){
            output=true;
            command->answer("output is now turned on\n", this->v_shared_from_this());
        }
        else if(command->getArguments()=="off"){
            output=false;
            command->answer("output is now turned off\n", this->v_shared_from_this());
        }
        else{
            command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n", this->v_shared_from_this());
        }
    }
    else if(command->getCommand()=="mirror"){
        if(command->getArguments()=="on"){
            portmirror=true;
            command->answer("mirroring of messages is now turned on\n", this->v_shared_from_this());
        }
        else if(command->getArguments()=="off"){
            portmirror=false;
            command->answer("mirroring of messages is now turned off\n", this->v_shared_from_this());
        }
        else{
            command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n", this->v_shared_from_this());
        }
    }
    else if(command->getCommand()=="checksum"){
        if(command->getArguments()=="on"){
            checksum=true;
            command->answer("checksum calculation is now turned on\n", this->v_shared_from_this());
        }
        else if(command->getArguments()=="off"){
            checksum=false;
            command->answer("checksum calculation is now turned off\n", this->v_shared_from_this());
        }
        else{
            command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n", this->v_shared_from_this());
        }
    }
    else{
        command->answer(Answer::UNKNOWN_CMD, "Cannot understand command "+command->getCommand()+"\n", this->v_shared_from_this());
    }
    //TODO: response generieren und bei endpoint delivern
}