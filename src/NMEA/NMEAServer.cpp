//
//  Server.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 22.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "NMEAServer.h"
#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "../TCP/TCPServer.h"
#include "../File/FileEndpoint.h"
#include "../Serial/SerialPort.h"
#include "../NMEA/GPSEndpoint.h"
#include "../NMEA/AISEndpoint.h"
#include "../NMEA/AIVDOEndpoint.h"
#include "../NMEA/MemoryStoreEndpoint.h"
#include "../NMEA/CompassEndpoint.h"
#include "../NMEA/TimeEndpoint.h"

static NMEAServer_ptr theInstance = NMEAServer_ptr();

NMEAServer_ptr NMEAServer::getInstance() {
    
    if(theInstance==NULL){
        theInstance = NMEAServer_ptr(new NMEAServer());
        theInstance->initialize();
    }
    return theInstance;
}

NMEAServer::NMEAServer(): io_service_pool_(1){
    shouldRun=false;
}

void NMEAServer::addEndpoint(Endpoint_ptr endpoint){
    boost::mutex::scoped_lock lock(offlineMutex);
    offline.push_back(endpoint);
}
void NMEAServer::removeEndpoint(Endpoint_ptr endpoint){
    boost::mutex::scoped_lock lock(offlineMutex);
    offline.remove(endpoint);
}

void NMEAServer::endpointOnline(Endpoint_ptr endpoint){
    {
        boost::mutex::scoped_lock lock(offlineMutex);
        offline.remove(endpoint);
    }
    {
        boost::mutex::scoped_lock lock(onlineMutex);
        online.push_back(endpoint);
    }
    std::ostringstream oss;
    oss << "Client registred, now having " << online.size() << " ("<<online.size()+offline.size()<<") clients";
    log(oss.str());
}
void NMEAServer::endpointOffline(Endpoint_ptr endpoint){
    {
        boost::mutex::scoped_lock lock(onlineMutex);
        online.remove(endpoint);
    }
    {
        boost::mutex::scoped_lock lock(offlineMutex);
        offline.push_back(endpoint);
    }
    std::ostringstream oss;
    oss << "Client unegistred, now having " << online.size() << " ("<<online.size()+offline.size()<<") clients";
    log(oss.str());
}

void NMEAServer::receive(Command_ptr msg){
    receive((Message_ptr)msg);
}
void NMEAServer::receive(NMEAmsg_ptr msg){
    receive((Message_ptr)msg);
}
void NMEAServer::receive(Answer_ptr msg){
    receive((Message_ptr)msg);
}
void NMEAServer::receive(Message_ptr msg){
    boost::mutex::scoped_lock lock(msgsMutex);
    msgs.push_back(msg);
    msgsCond.notify_one();
}

void NMEAServer::deliver(Command_ptr command){
    if((*command).getReceiver()=="server"){
        receiveCommand(command);
    }
    else{
        receive(command);
    }
}

void NMEAServer::receiveCommand(Command_ptr command){
        if(command->getCommand()=="exit" || command->getCommand()=="logout" || command->getCommand()=="close"){
            //TODO call exit on all endpoints
            raise(SIGTERM);
        }
        else if(command->getCommand()=="endpoints"){
            std::ostringstream ss;
            ss << "Currently " << online.size() << " endpoints connected" << std::endl << "---------------------------------------" << std::endl;
            for (std::list<Endpoint_ptr>::const_iterator endpoint = online.begin(), end = online.end(); endpoint != end; ++endpoint) {
                std::string state = getEndpointState();
                if(state.length()>0){
                    state = "("+state+")";
                }
                ss << (*endpoint)->getId() << '\t' << state << std::endl;
            }
            command->answer(ss.str(), this->shared_from_this());
        }
        else if(command->getCommand()=="time"){
            command->answer(to_simple_string(boost::posix_time::microsec_clock::local_time())+"\n", this->shared_from_this());
        }
        else if(command->getCommand()=="add"){
            boost::regex reg("^([[:alnum:]]*)\\h?$");
            boost::regex regArg("^([[:alnum:]]*)\\s([[:print:]]+)\\h?$");
            boost::cmatch matches;
            std::string type;
            std::string args;
            bool matched=false;
            if(boost::regex_search(command->getArguments().c_str(), matches, regArg)){
                type=std::string(matches[1].first, matches[1].second);
                args=std::string(matches[2].first, matches[2].second);
                matched=true;
            }
            else if(boost::regex_search(command->getArguments().c_str(), matches, reg)){
                type=std::string(matches[1].first, matches[1].second);
                args=std::string("");
                matched=true;
            }
            if(matched){
                if(type=="file" || type=="File"){
                    try
                    {
                        FileEndpoint::factory(this->shared_from_this(), args);
                        command->answer("New file Endpoint successfully created\n", this->shared_from_this());
                    }
                    catch (std::exception& e)
                    {
                        std::ostringstream oss;
                        oss << "FileEndpoint Exception: " << e.what();
                        log(oss.str());
                    }
                }
                else if(type=="tcp" || type=="TCP"){
                    try
                    {
                        TCPServer::factory(this->shared_from_this(), std::atoi(args.c_str()));
                        command->answer("New tcp Endpoint successfully created\n", this->shared_from_this());
                    }
                    catch (std::exception& e)
                    {
                        std::ostringstream oss;
                        oss << "TCPSession Exception: " << e.what();
                        log(oss.str());
                    }
                }
                else if(type=="serial"){
                    std::vector<std::string> strs;
                    try
                    {
                        boost::split(strs, args, boost::is_any_of("\t "));
                        if(strs.size()==2){
                            unsigned int boud = lexical_cast<unsigned int>(strs[1]);
                            SerialPort::factory(this->shared_from_this(), strs[0] ,boud);
                            command->answer("New SerialPort successfully created\n", this->shared_from_this());
                        }
                        else{
                            command->answer(Answer::WRONG_ARGS,"Cannot create serial. Argument must be \"device boudrate\"\n", this->shared_from_this());
                        }
                    }
                    catch (std::exception& e)
                    {
                        std::cerr << "serial: " << e.what() << "\n";
                        command->answer(Answer::WRONG_ARGS,"Cannot create serial. Argument must be \"device boudrate\"\n", this->shared_from_this());
                    }
                }
                else if(type=="GPSreceiver"){
                    GPSEndpoint::factory(this->shared_from_this());
                    command->answer("New GPSEndpoint successfully created\n", this->shared_from_this());
                }
                else if(type=="TimeEndpoint"){
                    TimeEndpoint::factory(this->shared_from_this());
                    command->answer("New TimeEndpoint successfully created\n", this->shared_from_this());
                }
                else if(type=="AISreceiver"){
                    AISEndpoint::factory(this->shared_from_this());
                    command->answer("New AISEndpoint successfully created\n", this->shared_from_this());
                }
                else if(type=="virtualAIVDO"){
                    try
                    {
                        if(args.length()==9){
                            unsigned int mmsi = lexical_cast<unsigned int>(args);
                            AIVDOEndpoint::factory(this->shared_from_this(), mmsi);
                            command->answer("New virtualAIVDO successfully created\n", this->shared_from_this());
                            
                        }
                        else{
                            command->answer(Answer::WRONG_ARGS,"Cannot create virtualAIVDO. MMSI Arguemnt must be 9 digits\n", this->shared_from_this());
                        }
                    }
                    catch (std::exception& e)
                    {
                        std::cerr << "virtualAIVDO: " << e.what() << "\n";
                        command->answer(Answer::WRONG_ARGS,"Cannot create virtualAIVDO. MMSI Arguemnt must be 9 digits\n", this->shared_from_this());
                    }
                }
                else if(type=="MemoryStore"){
                    MemoryStoreEndpoint::factory(this->shared_from_this(), args);
                    command->answer("New MemoryStoreEndpoint successfully created\n", this->shared_from_this());
                }
                else if(type=="Compass"){
                    CompassEndpoint::factory(this->shared_from_this());
                    command->answer("New CompassEndpoint successfully created\n", this->shared_from_this());
                }
                else{
                    command->answer(Answer::WRONG_ARGS, "Cannot understand command Argument "+command->getArguments()+" for Command "+command->getCommand()+". No such endpoint type to create.\n", this->shared_from_this());
                }
            }
            else{
                command->answer(Answer::WRONG_ARGS, "Cannot understand command Argument "+command->getArguments()+" for Command "+command->getCommand()+"\n", this->shared_from_this());
            }
        }
        else{
            CommandEndpoint::receive(command);
        }
}

void NMEAServer::run(){
    shouldRun=true;
    while(shouldRun){
        std::list<Message_ptr> msgs_cpy;
        {
            boost::mutex::scoped_lock lock(msgsMutex);
            if(msgs.empty()){
                msgsCond.wait(lock);
            }
            msgs_cpy = std::list<Message_ptr>(msgs);
        }
        while(!msgs_cpy.empty()){
            std::list<Endpoint_ptr> online_cpy;
            {
                boost::mutex::scoped_lock lock(onlineMutex);
                online_cpy = std::list<Endpoint_ptr>(online);
            }
            if(NMEAmsg_ptr tmp_msg = boost::dynamic_pointer_cast<NMEAmsg>(msgs_cpy.front())){
                for (std::list<Endpoint_ptr>::const_iterator endpoint = online_cpy.begin(), end = online_cpy.end(); endpoint != end; ++endpoint) {
                    NMEAEndpoint_ptr nmeaEnd = boost::dynamic_pointer_cast<NMEAEndpoint>(*endpoint);
                    if(nmeaEnd){
                        nmeaEnd->deliver(tmp_msg);
                    }
                }
            }
            else if(Command_ptr tmp_cmd = boost::dynamic_pointer_cast<Command>(msgs_cpy.front())){
                if(tmp_cmd->getReceiver()=="*"){
                    receiveCommand(tmp_cmd);
                }
                if(tmp_cmd->getReceiver()=="server"){
                    receiveCommand(tmp_cmd);
                }
                else{
                    bool found=false;
                    for (std::list<Endpoint_ptr>::const_iterator endpoint = online_cpy.begin(), end = online_cpy.end(); endpoint != end; ++endpoint) {
                        std::string receiver = tmp_cmd->getReceiver();
                        boost::replace_all(receiver, "*", "(.*)");
                        boost::regex reg("^"+receiver+"$");
                        boost::cmatch matches;
                        CommandEndpoint_ptr commandEnd = boost::dynamic_pointer_cast<CommandEndpoint>(*endpoint);
                        if(commandEnd){
                            if(boost::regex_search(commandEnd->getId().c_str(), matches, reg)){
                                if(tmp_cmd->getSender()!=commandEnd){
                                    commandEnd->receive(tmp_cmd);
                                    found=true;
                                }
                            }
                        }
                    }
                    if(!found){
                        tmp_cmd->answer(Answer::UNKNOWN_RECEIVER, "The receiver "+tmp_cmd->getReceiver()+" was not found \n", this->shared_from_this());
                    }
                }
            }
            else if(Answer_ptr tmp_answer = boost::dynamic_pointer_cast<Answer>(msgs_cpy.front())){
                if(tmp_answer->getReceiver()=="*"){
                    deliver(tmp_answer);
                }
                if(tmp_answer->getReceiver()=="server"){
                    deliver(tmp_answer);
                }
                else{
                    bool found=false;
                    for (std::list<Endpoint_ptr>::const_iterator endpoint = online_cpy.begin(), end = online_cpy.end(); endpoint != end; ++endpoint) {
                        std::string receiver = tmp_answer->getReceiver();
                        boost::replace_all(receiver, "*", "(.*)");
                        boost::regex reg("^"+receiver+"$");
                        boost::cmatch matches;
                        CommandEndpoint_ptr commandEnd = boost::dynamic_pointer_cast<CommandEndpoint>(*endpoint);
                        if(commandEnd){
                            if(boost::regex_search(commandEnd->getId().c_str(), matches, reg)){
                                if(tmp_answer->getSender()!=commandEnd){
                                    commandEnd->deliver(tmp_answer);
                                    found=true;
                                }
                            }
                        }
                    }
                }
            }
            {
                boost::mutex::scoped_lock lock(msgsMutex);
                msgs.remove(msgs_cpy.front());
            }
            msgs_cpy.pop_front();
        }
    }
}

void NMEAServer::stop(){
    shouldRun=false;
    msgsCond.notify_one();
}
