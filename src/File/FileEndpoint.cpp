//
//  FileEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 26.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "FileEndpoint.h"
#include "../NMEA/NMEAServer.h"
#include <boost/regex.hpp>

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
    else if(command->getCommand()=="restart"){
        file_stream.seekg(0, file_stream.beg);
        command->getSender()->deliverAnswer("playback of "+getId()+" restarted at beginning\n");
    }
    else if(command->getCommand()=="stop"){
        stopPlayback();
        command->getSender()->deliverAnswer("playback of "+getId()+" stopped\n");
    }
    else if(command->getCommand()=="play"){
        startPlayback(boost::posix_time::microsec_clock::local_time(),boost::posix_time::microsec_clock::local_time());
        command->getSender()->deliverAnswer("playback of "+getId()+" started\n");
    }
    else if(command->getCommand()=="record"){
        record();
        command->getSender()->deliverAnswer("record to "+getId()+" started\n");
    }
    else{
        NMEAEndpoint::receiveCommand(command);
    }
}

void FileEndpoint::deliver_impl(NMEAmsg_ptr msg){
    if(!playback){
        file_stream<<  to_simple_string(msg->getReceived()) << " " << msg->data(true);
        file_stream.flush();
    }
}

void FileEndpoint::record(){
    stopPlayback();
    file_stream.seekg(0, file_stream.end);
    playback=false;
}

void FileEndpoint::startPlayback(boost::posix_time::ptime from,  boost::posix_time::ptime to){
    playback=true;
    stopPlaybackNow=false;
    playbackThread = boost::thread(&FileEndpoint::play, this, from, to);
}

void FileEndpoint::stopPlayback(){
    stopPlaybackNow=true;
}

//TODO Implement search and change find and add regex for lines without time
void FileEndpoint::play(boost::posix_time::ptime from,  boost::posix_time::ptime to){
    
    file_stream.seekg(0, file_stream.beg);
    
    //boost::regex reg("^([^ ]+\\s[^ ]+)\\s([[:print:]]+)\\h?");
    boost::regex reg("^(\\d{4}-.*-\\d{2}\\s\\d{2}:\\d{2}:\\d{2}.\\d{1,10}\\s)?([[:print:]]+)\\h?");
    boost::cmatch matches;
    std::string line;
    while(!stopPlaybackNow && std::getline(file_stream,line)){
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if(boost::regex_search(line.c_str(), matches, reg)){
            std::string date=std::string(matches[1].first, matches[1].second);
            std::string msg_or_cmd=std::string(matches[2].first, matches[2].second);
            if(msg_or_cmd.find('#')!=std::string::npos){
                try {
                    Command_ptr command(new Command(msg_or_cmd, this->shared_from_this()));
                    deliverCommand(command);
                }
                catch (const std::invalid_argument& ia) {
                    std::cerr << "Received a command that might not be conform with command format: " << line << std::endl;
                }
            }
            else if(msg_or_cmd.find('$')!=std::string::npos || msg_or_cmd.find('!')!=std::string::npos){
                if(input){
                    try {
                        NMEAmsg_ptr msg(new NMEAmsg(msg_or_cmd, this->shared_from_this()));
                        receive(msg);
                    }
                    catch (const std::invalid_argument& ia) {
                        std::cerr << "Received a message that might not be conform with NMEA-0183 message format: " << line << std::endl;
                    }
                }
            }
            else{
                std::cerr << "Dont understand message or command:" << msg_or_cmd <<std::endl;
            }
        }
        else{
            std::cerr << "Dont understand line:" << line <<std::endl;
        }
        //TODO Better with timer!
        sleep(1);
    }
    stopPlayback();
}

void FileEndpoint::deliverAnswer_impl(std::string answer){
    //TODO: maybe write answers to file?
}

FileEndpoint::FileEndpoint(){
    playback=false;
    stopPlaybackNow=true;
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
    registerEndpoint(); }

void FileEndpoint::close(){
    unregisterEndpoint();
    file_stream.close();
}
