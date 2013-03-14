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

void NMEAEndpoint::deliverAnswer(std::string answer){
    deliverAnswer_impl(answer);
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

void NMEAEndpoint::deliverCommand(Command_ptr command){
    if((*command).getReceiver()=="local"){
        receiveCommand(command);
    }
    else{
        NMEAServer::getInstance()->receiveCommand(command);
    }
}

void NMEAEndpoint::receiveCommand(Command_ptr command){
    //TODO Helpermethoden damit das nicht ewig lang wird hier!
    if(command->getCommand()=="id"){
        command->getSender()->deliverAnswer(getId()+"\n");
    }
    else if(command->getCommand()=="input"){
        if(command->getArguments()=="on"){
            input=true;
            command->getSender()->deliverAnswer("input is now turned on\n");
        }
        else if(command->getArguments()=="off"){
            input=false;
            command->getSender()->deliverAnswer("input is now turned off\n");
        }
        else{
            command->getSender()->deliverAnswer("Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n");
        }
    }
    else if(command->getCommand()=="output"){
        if(command->getArguments()=="on"){
            output=true;
            command->getSender()->deliverAnswer("output is now turned on\n");
        }
        else if(command->getArguments()=="off"){
            output=false;
            command->getSender()->deliverAnswer("output is now turned off\n");
        }
        else{
            command->getSender()->deliverAnswer("Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n");
        }
    }
    else if(command->getCommand()=="mirror"){
        if(command->getArguments()=="on"){
            portmirror=true;
            command->getSender()->deliverAnswer("mirroring of messages is now turned on\n");
        }
        else if(command->getArguments()=="off"){
            portmirror=false;
            command->getSender()->deliverAnswer("mirroring of messages is now turned off\n");
        }
        else{
            command->getSender()->deliverAnswer("Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n");
        }
    }
    else if(command->getCommand()=="checksum"){
        if(command->getArguments()=="on"){
            checksum=true;
            command->getSender()->deliverAnswer("checksum calculation is now turned on\n");
        }
        else if(command->getArguments()=="off"){
            checksum=false;
            command->getSender()->deliverAnswer("checksum calculation is now turned off\n");
        }
        else{
            command->getSender()->deliverAnswer("Cannot understand "+command->getArguments()+" for command "+command->getCommand()+"\n");
        }
    }
    else{
        command->getSender()->deliverAnswer("Cannot understand command "+command->getCommand()+"\n");
    }
    //TODO: response generieren und bei endpoint delivern
}