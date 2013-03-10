//
//  FileEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 26.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "FileEndpoint.h"
#include "../NMEA/NMEAServer.h"


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

void FileEndpoint::open(char *filename){
    this->filename = std::string(filename);
    file_stream.open(filename);
    NMEAServer::getInstance()->addEndpoint(shared_from_this());
 }

void FileEndpoint::close(){
    file_stream.close();
    NMEAServer::getInstance()->removeEndpoint(shared_from_this());
}
