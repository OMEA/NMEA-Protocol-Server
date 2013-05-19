//
//  CommandEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 14.03.13.
//
//

#include "CommandEndpoint.h"
#include "../NMEA/NMEAServer.h"

#include <boost/bind.hpp>

CommandEndpoint::CommandEndpoint(boost::shared_ptr<Endpoint> connectedTo): Endpoint(connectedTo){}

void CommandEndpoint::initialize(){
    Endpoint::initialize();
    boost::function<void (Command_ptr)> func = boost::bind(&CommandEndpoint::id_cmd, this, _1);
    registerVoidCmd("id","Identifier", "The identifier of the endpoint. The property is read-only.",  func);
    registerStringVectorCmd("log_destination","Destination to log to", "Define an end point to which log messages are sent.",  &log_destination, true);
}

void CommandEndpoint::id_cmd(Command_ptr command){
    command->answer(getId()+"\n", this->v_shared_from_this());
}

void CommandEndpoint::deliver(Command_ptr command){
    if((*command).getReceiver()=="local"){
        receive(command);
    }
    else{
        if(!getConnectedTo()){
            std::cerr << getId()<<": Endpoint is unconnected" << std::endl;
        }
        else{
            CommandEndpoint_ptr cmdEndpoint = boost::dynamic_pointer_cast<CommandEndpoint>(getConnectedTo());
            if(cmdEndpoint){
                cmdEndpoint->deliver(command);
            }
            else{
                std::cerr << getId()<<":deliver Cannot send Command to none CommandEndpoint" << std::endl;
            }
        }
    }
}

void CommandEndpoint::receive(Answer_ptr answer){
    if((*answer).getReceiver()=="local"){
        deliver(answer);
    }
    else{
        if(!getConnectedTo()){
            std::cerr << getId()<<": Endpoint is unconnected" << std::endl;
        }
        else{
            CommandEndpoint_ptr cmdEndpoint = boost::dynamic_pointer_cast<CommandEndpoint>(getConnectedTo());
            if(cmdEndpoint){
                cmdEndpoint->receive(answer);
            }
            else{
                std::cerr << getId()<<":deliver Cannot send Answer to none CommandEndpoint" << std::endl;
            }
        }
    }
    
}

void CommandEndpoint::deliver(Answer_ptr answer){
    deliverAnswer_impl(answer);
}

void CommandEndpoint::receive(Command_ptr command){
    if(command->getCommand()=="list_commands"){
        std::ostringstream ss;
        ss << "you can call the following commands on "<<getId()<<std::endl << "---------------------------------------" << std::endl;
        for (std::map<std::string,Callback_ptr>::iterator registred_command=registred_commands.begin(); registred_command!=registred_commands.end(); ++registred_command){
            ss << *(registred_command->second) << std::endl;
        }
        command->answer(ss.str(), this->v_shared_from_this());
    }
    else if(registred_commands.find(command->getCommand())!=registred_commands.end()){
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

void CommandEndpoint::unregisterCmd(std::string name){
    registred_commands.erase(name);
}

void CommandEndpoint::registerBoolCmd(std::string name, std::string speakingName, std::string description, bool *boolean, bool defaultValue, bool writeable){
    *boolean=defaultValue;
    Callback_ptr callback(new BoolCallback(name, speakingName, description, boolean, writeable));
    registred_commands.insert(std::pair<std::string,Callback_ptr>(name,callback));
}

void CommandEndpoint::registerUIntCmd(std::string name, std::string speakingName, std::string description, unsigned long long int *uint, unsigned long long int defaultValue, unsigned long long int min, unsigned long long int max, bool writeable){
    *uint=defaultValue;
    Callback_ptr callback(new UIntCallback(name, speakingName, description, uint, min, max, writeable));
    registred_commands.insert(std::pair<std::string,Callback_ptr>(name,callback));
}

void CommandEndpoint::registerIntCmd(std::string name, std::string speakingName, std::string description, long long int *int_, long long int defaultValue, long long int min, long long int max, bool writeable){
    *int_=defaultValue;
    Callback_ptr callback(new IntCallback(name, speakingName, description, int_, min, max, writeable));
    registred_commands.insert(std::pair<std::string,Callback_ptr>(name,callback));
}

void CommandEndpoint::registerStringVectorCmd(std::string name, std::string speakingName, std::string description, std::vector<std::string> *vector, bool writeable){
    Callback_ptr callback(new StringVectorCallback(name, speakingName, description, vector, writeable));
    registred_commands.insert(std::pair<std::string,Callback_ptr>(name,callback));
}

void CommandEndpoint::registerStringCmd(std::string name, std::string speakingName, std::string description, std::string *string, std::string defaultValue, bool writeable){
    *string = defaultValue;
    Callback_ptr callback(new StringCallback(name, speakingName, description, string, writeable));
    registred_commands.insert(std::pair<std::string,Callback_ptr>(name,callback));
}

void CommandEndpoint::registerVoidCmd(std::string name, std::string speakingName, std::string description, boost::function<void (Command_ptr)> &func){
    Callback_ptr callback(new VoidCallback(name, speakingName, description, func));
    registred_commands.insert(std::pair<std::string,Callback_ptr>(name,callback));
}

void CommandEndpoint::log(std::string message){
    if(log_destination.size()>0){
        for(std::vector<std::string>::iterator destination = log_destination.begin(); destination!=log_destination.end();++destination){
            Answer_ptr answer = Answer_ptr(new Answer(Answer::LOG, message, v_shared_from_this(), Command_ptr()));
            answer->setReceiver(*destination);
            receive(answer);
        }
    }
    else {
        std::cout << message << std::endl;
    }
}