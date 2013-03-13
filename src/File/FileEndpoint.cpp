//
//  FileEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 26.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "FileEndpoint.h"
#include "../NMEA/NMEAServer.h"

boost::shared_ptr<FileEndpoint> FileEndpoint::factory(std::string filename) {
    FileEndpoint_ptr fileEndpoint(new FileEndpoint);
    fileEndpoint->open(filename);
    return fileEndpoint;
}

void FileEndpoint::receiveCommand(Command_ptr command){
    if(command->getCommand()=="exit" || command->getCommand()=="logout" || command->getCommand()=="close"){
        close();
        command->getSender()->deliverAnswer("closed file "+filename+"\n");
    }
    else{
        NMEAEndpoint::receiveCommand(command);
    }
}

void FileEndpoint::deliver_impl(NMEAmsg_ptr msg){
    file_stream<<  to_simple_string(msg->getReceived()) << " " << msg->data();
    file_stream.flush();
}

void FileEndpoint::deliverAnswer_impl(std::string answer){
    //TODO: maybe write answers to file?
}

FileEndpoint::FileEndpoint(){
}

FileEndpoint::~FileEndpoint(){
    //close();
}

std::string FileEndpoint::getId(){
    return "File://"+filename;
}

void FileEndpoint::open(std::string filename){
    this->filename = filename;
    file_stream.open(filename.c_str());
    NMEAServer::getInstance()->addEndpoint(shared_from_this());
 }

void FileEndpoint::close(){
    NMEAServer::getInstance()->removeEndpoint(shared_from_this());
    file_stream.close();
}
