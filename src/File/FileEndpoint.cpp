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
#include <boost/filesystem.hpp>

boost::shared_ptr<FileEndpoint> FileEndpoint::factory(boost::shared_ptr<Endpoint> connectedTo, std::string filename) {
    FileEndpoint_ptr fileEndpoint(new FileEndpoint(connectedTo));
    fileEndpoint->initialize();
    fileEndpoint->open(filename);
    return fileEndpoint;
}

void FileEndpoint::initialize(){
    NMEAEndpoint::initialize();
    registerIntCmd("min_available","Minimimum remaining disk space", "Defines the minimum remaining disk space that should be left. When there is not enough space on the disk hosting this file, nothing is written. 0 = nocheck", &min_available_mbs, 0, 0, std::numeric_limits<int>::max(), true);
    boost::function<void (Command_ptr)> func = boost::bind(&FileEndpoint::space_left_cmd, this, _1);
    registerVoidCmd("space_left","Print Space left", "Prints the space left on the disk",  func);
}

void FileEndpoint::space_left_cmd(Command_ptr command){
    boost::filesystem::path filepath(filename);
    boost::filesystem::space_info space = boost::filesystem::space(filepath);
    std::ostringstream oss;
    oss << (space.available/1024/1024) << "MB left on device\n";
    command->answer(oss.str(), this->shared_from_this());
}

void FileEndpoint::receive(Command_ptr command){
    if(command->getCommand()=="exit" || command->getCommand()=="logout" || command->getCommand()=="close"){
        FileEndpoint_ptr this_ptr = this->shared_from_this();
        close();
        command->answer("closed file "+filename+"\n", this_ptr);
    }
    else if(command->getCommand()=="restart"){
        in_file_stream.seekg(0, in_file_stream.beg);
        command->answer("playback of "+getId()+" restarted at beginning\n", this->shared_from_this());
    }
    else if(command->getCommand()=="stop"){
        stopPlayback();
        command->answer("playback of "+getId()+" stopped\n", this->shared_from_this());
    }
    else if(command->getCommand()=="play"){
        startPlayback(boost::posix_time::microsec_clock::local_time(),boost::posix_time::microsec_clock::local_time());
        command->answer("playback of "+getId()+" started\n", this->shared_from_this());
    }
    else if(command->getCommand()=="record"){
        record();
        command->answer("record to "+getId()+" started\n", this->shared_from_this());
    }
    else{
        NMEAEndpoint::receive(command);
    }
}

void FileEndpoint::deliver_impl(NMEAmsg_ptr msg){
    if(recording){
        if(min_available_mbs > 0){
            boost::filesystem::path filepath(filename);
            boost::filesystem::space_info space = boost::filesystem::space(filepath);
            if(space.available != -1 && min_available_mbs != -1 && ((space.available/1024/1024)<=min_available_mbs)){
                if(!wasFull){
                    std::ostringstream oss;
                    oss << "Space available not sufficient:" << space.available/1024/1024;
                    log(oss.str());
                }
                wasFull=true;
                return;
            }
            else{
                 wasFull=false;
            }
        }
        out_file_stream << to_simple_string(msg->getReceived()) << " " << msg->data(true);
        out_file_stream.flush();
    }
}

void FileEndpoint::record(){
    recording=true;
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
    in_file_stream.clear();
    in_file_stream.seekg(0, in_file_stream.beg);
    
    //boost::regex reg("^([^ ]+\\s[^ ]+)\\s([[:print:]]+)\\h?");
    boost::regex reg("^(\\d{4}-.*-\\d{2}\\s\\d{2}:\\d{2}:\\d{2}.\\d{1,10}\\s)?([[:print:]]+)\\h?");
    boost::cmatch matches;
    std::string line;
    while(!stopPlaybackNow && std::getline(in_file_stream, line)){
        line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
        line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
        if(boost::regex_search(line.c_str(), matches, reg)){
            std::string date=std::string(matches[1].first, matches[1].second);
            std::string msg_or_cmd=std::string(matches[2].first, matches[2].second);
            if(msg_or_cmd.find('#')!=std::string::npos){
                try {
                    Command_ptr command(new Command(msg_or_cmd, this->shared_from_this()));
                    CommandEndpoint::deliver(command);
                }
                catch (const std::invalid_argument& ia) {
                    std::cerr << "Received a command that might not be conform with command format: " << line << std::endl;
                }
            }
            else if(msg_or_cmd.find('$')!=std::string::npos || msg_or_cmd.find('!')!=std::string::npos){
                if(input){
                    try {
                        NMEAmsg_ptr msg = NMEAmsg::factory(msg_or_cmd, this->shared_from_this(), check_checksum);
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

void FileEndpoint::deliverAnswer_impl(Answer_ptr answer){
    //TODO: maybe write answers to file?
}

FileEndpoint::FileEndpoint(boost::shared_ptr<Endpoint> connectedTo): NMEAEndpoint(connectedTo){
    recording=false;
    playback=false;
    stopPlaybackNow=true;
}

FileEndpoint::~FileEndpoint(){
}

std::string FileEndpoint::getId(){
    return "File://"+filename;
}

void FileEndpoint::open(std::string filename){
    this->filename = filename;
    in_file_stream.open(filename.c_str());
    out_file_stream.open(filename.c_str(), std::ios_base::app);
    registerEndpoint();
}

void FileEndpoint::close(){
    NMEAServer::getInstance()->endpointOffline(this->v_shared_from_this());
    in_file_stream.close();
    out_file_stream.close();
    NMEAServer::getInstance()->removeEndpoint(this->v_shared_from_this());
}
