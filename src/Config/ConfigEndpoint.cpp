//
//  FileEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 26.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <string>

#include <boost/regex.hpp>

#include "ConfigEndpoint.h"
#include "../NMEA/NMEAServer.h"

boost::shared_ptr<ConfigEndpoint> ConfigEndpoint::factory(std::string configname) {
    ConfigEndpoint_ptr configEndpoint(new ConfigEndpoint);
    configEndpoint->load(configname);
    return configEndpoint;
}

void ConfigEndpoint::receive(Command_ptr command){
    if(command->getCommand()=="exit" || command->getCommand()=="logout" || command->getCommand()=="close"){
        ConfigEndpoint_ptr this_ptr = this->shared_from_this();
        close();
        command->answer("closed config "+configname+"\n", this_ptr);
    }
    else if(command->getCommand()=="has_commands"){
        for (std::list<Command_ptr>::const_iterator tmp_command = commands.begin(), end = commands.end(); tmp_command != end; ++tmp_command) {
            std::string receiver = (*tmp_command)->getReceiver();
            boost::replace_all(receiver, "*", "(.*)");
            boost::regex reg("^"+receiver+"$");
            boost::cmatch matches;
            CommandEndpoint_ptr sender = boost::dynamic_pointer_cast<CommandEndpoint>(command->getSender());
            if(sender){
                if(boost::regex_search(sender->getId().c_str(), matches, reg)){
                    sender->receive((*tmp_command));
                }
            }
        }
    }
    else if(command->getCommand()=="add_command"){
        try {
            Command_ptr newCommand(new Command(command->getArguments(), this->shared_from_this()));
            commands.push_back(newCommand);
            deliver(newCommand);
        }
        catch (const std::invalid_argument& ia) {
            command->answer(Answer::UNKNOWN_CMD, "Command might not be conform with command format "+command->getArguments()+"\n", this->shared_from_this());
        }
        command->answer("added command "+command->getArguments()+"\n", this->shared_from_this());
    }
    else if(command->getCommand()=="save"){
        if(command->getArguments().length()>0){
            save(command->getArguments());
            command->answer("saved config to "+command->getArguments()+"\n", this->shared_from_this());
        }
        else{
            save(configname);
            command->answer("saved config to "+configname+"\n", this->shared_from_this());
        }
            
    }
    else if(command->getCommand()=="list"){
        std::stringstream ss;
        ss << "Currently " << commands.size() << " commands in config" << std::endl << "---------------------------------------" << std::endl;
        for (std::list<Command_ptr>::const_iterator commandi = commands.begin(), end = commands.end(); commandi != end; ++commandi) {
            ss << (*commandi)->to_str() << '\n';
        }
        command->answer(ss.str(), this->shared_from_this());
    }
    else{
        CommandEndpoint::receive(command);
    }
}

void ConfigEndpoint::deliverAnswer_impl(Answer_ptr answer){
    if(answer->getType()!=Answer::UNKNOWN_RECEIVER){
        std::cout<<getId()<<":"<<*answer;
    }
}

ConfigEndpoint::ConfigEndpoint(){
}

ConfigEndpoint::~ConfigEndpoint(){
    //close();
}

std::string ConfigEndpoint::getId(){
    return "Config://"+configname;
}

void ConfigEndpoint::load(std::string configname){
    this->configname = configname;
    std::ifstream file_stream;
    file_stream.open(configname.c_str());
    //TODO: parsen und einfügen
    std::string line;
    while(std::getline(file_stream,line)){
        if(line.find('#')!=std::string::npos){
            try {
                Command_ptr command(new Command(line, this->shared_from_this()));
                commands.push_back(command);
            }
            catch (const std::invalid_argument& ia) {
                std::cerr << "Received a command that might not be conform with command format: " << line << std::endl;
            }
        }
    }
    file_stream.close();
    registerEndpoint();
    for (std::list<Command_ptr>::const_iterator command = commands.begin(), end = commands.end(); command != end; ++command) {
        std::cout<<getId()<<":"<<(**command)<<std::endl;
        deliver(*command);
    }
 }

void ConfigEndpoint::save(std::string configname){
    std::ofstream file_stream;
    file_stream.open(configname.c_str());
    for (std::list<Command_ptr>::const_iterator command = commands.begin(), end = commands.end(); command != end; ++command) {
        file_stream << (*command)->to_str() << '\n';
    }
    file_stream.close();
}

void ConfigEndpoint::close(){
    unregisterEndpoint();
}
