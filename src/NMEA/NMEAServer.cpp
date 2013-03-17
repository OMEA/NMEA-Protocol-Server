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

NMEAServer_ptr NMEAServer::getInstance() {
    static NMEAServer_ptr theInstance(new NMEAServer());
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
    std::cout << "Client registred, now having " << online.size() << " ("<<online.size()+offline.size()<<") clients" << std::endl;
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
    
    std::cout << "Client unregistred, now having " << online.size() << " ("<<online.size()+offline.size()<<") clients" << std::endl;
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

void NMEAServer::receiveCommand(Command_ptr command){
        if(command->getCommand()=="exit" || command->getCommand()=="logout" || command->getCommand()=="close"){
            //TODO call exit on all endpoints
            raise(SIGTERM);
        }
        else if(command->getCommand()=="endpoints"){
            std::stringstream ss;
            ss << "Currently " << online.size() << " endpoints connected" << std::endl << "---------------------------------------" << std::endl;
            for (std::list<Endpoint_ptr>::const_iterator endpoint = online.begin(), end = online.end(); endpoint != end; ++endpoint) {
                ss << (*endpoint)->getId() << std::endl;
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
                        std::cerr << "FileEndpoint Exception: " << e.what() << "\n";
                    }
                }
                else if(type=="tcp" || type=="TCP"){
                    try
                    {
//                        //IOService in endpoint und beim beenden stop() aufrufen
//                        boost::asio::io_service *io_service = new boost::asio::io_service();
//                        
//                        new TCPServer(this->shared_from_this(), *io_service, std::atoi(args.c_str()));
//                        //FileEndpoint::factory(args);
//                        boost::thread bt(boost::bind(&boost::asio::io_service::run, io_service));
                        TCPServer::factory(this->shared_from_this(), std::atoi(args.c_str()));
                        command->answer("New tcp Endpoint successfully created\n", this->shared_from_this());
                    }
                    catch (std::exception& e)
                    {
                        std::cerr << "TCP Exception: " << e.what() << "\n";
                    }
                }
            }
            else{
                command->answer(Answer::WRONG_ARGS, "Cannot understand command Argument "+command->getArguments()+" for Command "+command->getCommand()+"\n", this->shared_from_this());
            }
        }
        else{
            command->answer(Answer::UNKNOWN_CMD, "Cannot understand command "+command->getCommand()+"\n", this->shared_from_this());
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
