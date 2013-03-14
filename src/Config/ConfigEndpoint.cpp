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

#include "ConfigEndpoint.h"
#include "../NMEA/NMEAServer.h"

boost::shared_ptr<ConfigEndpoint> ConfigEndpoint::factory(std::string configname) {
    ConfigEndpoint_ptr configEndpoint(new ConfigEndpoint);
    configEndpoint->load(configname);
    return configEndpoint;
}

void ConfigEndpoint::receiveCommand(Command_ptr command){
    if(command->getCommand()=="exit" || command->getCommand()=="logout" || command->getCommand()=="close"){
        close();
        command->getSender()->deliverAnswer("closed config "+configname+"\n");
    }
    else if(command->getCommand()=="add_command"){
        try {
            Command_ptr newCommand(new Command(command->getArguments(), this->shared_from_this()));
            commands.push_back(newCommand);
            deliverCommand(newCommand);
        }
        catch (const std::invalid_argument& ia) {
            command->getSender()->deliverAnswer("Command might not be conform with command format "+command->getArguments()+"\n");
        }
        command->getSender()->deliverAnswer("added command "+command->getArguments()+"\n");
    }
    else if(command->getCommand()=="save"){
        if(command->getArguments().length()>0){
            save(command->getArguments());
            command->getSender()->deliverAnswer("saved config to "+command->getArguments()+"\n");
        }
        else{
            save(configname);
            command->getSender()->deliverAnswer("saved config to "+configname+"\n");
        }
            
    }
    else if(command->getCommand()=="list"){
        std::stringstream ss;
        ss << "Currently " << commands.size() << " commands in config" << std::endl << "---------------------------------------" << std::endl;
        for (std::list<Command_ptr>::const_iterator commandi = commands.begin(), end = commands.end(); commandi != end; ++commandi) {
            ss << (*commandi)->to_str() << '\n';
        }
        command->getSender()->deliverAnswer(ss.str());
    }
    else{
        NMEAEndpoint::receiveCommand(command);
    }
}

void ConfigEndpoint::deliver_impl(NMEAmsg_ptr msg){}

void ConfigEndpoint::deliverAnswer_impl(std::string answer){
    //TODO: maybe write answers to console?
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
        deliverCommand(*command);
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
