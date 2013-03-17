//
//  CommandEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 14.03.13.
//
//

#include "CommandEndpoint.h"
#include "../NMEA/NMEAServer.h"

CommandEndpoint::CommandEndpoint(boost::shared_ptr<Endpoint> connectedTo): Endpoint(connectedTo){
}

void CommandEndpoint::deliver(Command_ptr command){
    if((*command).getReceiver()=="local"){
        receive(command);
    }
    else{
        CommandEndpoint_ptr cmdEndpoint = boost::dynamic_pointer_cast<CommandEndpoint>(getConnectedTo());
        if(cmdEndpoint){
            cmdEndpoint->receive(command);
        }
        else{
            std::cerr << getId()<<":Cannot send Command to none CommandEndpoint" << std::endl;
        }
    }
}

void CommandEndpoint::deliver(Answer_ptr answer){
    deliverAnswer_impl(answer);
}

void CommandEndpoint::receive(Command_ptr command){
    if(registred_commands.find(command->getCommand())!=registred_commands.end()){
        std::pair<std::string,Callback_ptr> registred_command = *(registred_commands.find(command->getCommand()));
        registred_command.second->execute(command, boost::static_pointer_cast<CommandEndpoint>(this->v_shared_from_this()));
    }
    else{
        command->answer(Answer::UNKNOWN_CMD, "Cannot understand command "+command->getCommand()+"\n", this->v_shared_from_this());
    }
}

void CommandEndpoint::registerEndpoint(){
    Endpoint::registerEndpoint();
    //Ask for new commands
    Command_ptr has_commands(new Command("#Config:*#has_commands "+getId(), this->v_shared_from_this()));
    deliver(has_commands);
}

void CommandEndpoint::registerBoolCmd(std::string name, bool *boolean, bool defaultValue, bool writeable){
    *boolean=defaultValue;
    Callback_ptr callback(new BoolCallback(name, boolean, writeable));
    registred_commands.insert(std::pair<std::string,Callback_ptr>(name,callback));
}

void CommandEndpoint::registerUIntCmd(std::string name, unsigned int *uint, unsigned int defaultValue, unsigned int min, unsigned int max, bool writeable){
    *uint=defaultValue;
    Callback_ptr callback(new UIntCallback(name, uint, min, max, writeable));
    registred_commands.insert(std::pair<std::string,Callback_ptr>(name,callback));
}