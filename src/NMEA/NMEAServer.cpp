//
//  Server.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 22.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "NMEAServer.h"
#include <boost/regex.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "../TCP/TCPServer.h"
#include "../File/FileEndpoint.h"

NMEAServer* NMEAServer::getInstance() {
    static NMEAServer theInstance;
    return &theInstance;
}

NMEAServer::NMEAServer(): io_service_pool_(1){
    shouldRun=false;
}

void NMEAServer::addEndpoint(NMEAEndpoint_ptr endpoint){
    endpoints.push_back(endpoint);
    std::cout << "Client registred, now having " << endpoints.size() << " clients" << std::endl;
}
void NMEAServer::removeEndpoint(NMEAEndpoint_ptr endpoint){
    endpoints.remove(endpoint);
    std::cout << "Client unregistred, now having " << endpoints.size() << " clients" << std::endl;
}

void NMEAServer::receive(NMEAmsg_ptr msg){
    msgsMutex.lock();
    msgs.push(msg);
    msgsMutex.unlock();
    boost::mutex::scoped_lock lock(msgsMutex);
    msgsCond.notify_all();
}

void NMEAServer::receiveCommand(Command_ptr command){
    bool found=false;
    for (std::list<NMEAEndpoint_ptr>::const_iterator endpoint = endpoints.begin(), end = endpoints.end(); endpoint != end; ++endpoint) {
        std::string receiver = command->getReceiver();
        boost::replace_all(receiver, "*", "(.*)");
        boost::regex reg("^"+receiver+"$");
         boost::cmatch matches;
        if(boost::regex_search((*endpoint)->getId().c_str(), matches, reg)){
            (*endpoint)->receiveCommand(command);
            found=true;
        }
    }
    if(command->getReceiver()=="server"){
        if(command->getCommand()=="exit" || command->getCommand()=="logout" || command->getCommand()=="close"){
            //TODO call exit on all endpoints
            raise(SIGTERM);
        }
        else if(command->getCommand()=="endpoints"){
            std::stringstream ss;
            ss << "Currently " << endpoints.size() << " endpoints connected" << std::endl << "---------------------------------------" << std::endl;
            for (std::list<NMEAEndpoint_ptr>::const_iterator endpoint = endpoints.begin(), end = endpoints.end(); endpoint != end; ++endpoint) {
                ss << (*endpoint)->getId() << std::endl;
            }
            command->getSender()->deliverAnswer(ss.str());
        }
        else if(command->getCommand()=="time"){
            command->getSender()->deliverAnswer(to_simple_string(boost::posix_time::microsec_clock::local_time())+"\n");
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
                        FileEndpoint::factory(args);
                        command->getSender()->deliverAnswer("New file Endpoint successfully created");
                    }
                    catch (std::exception& e)
                    {
                        std::cerr << "FileEndpoint Exception: " << e.what() << "\n";
                    }
                }
                else if(type=="tcp" || type=="TCP"){
                    try
                    {
                        //IOService in endpoint und beim beenden stop() aufrufen
                        boost::asio::io_service *io_service = new boost::asio::io_service();
                        
                        new TCPServer(*io_service, std::atoi(args.c_str()));
                        //FileEndpoint::factory(args);
                        boost::thread bt(boost::bind(&boost::asio::io_service::run, io_service));
                        command->getSender()->deliverAnswer("New tcp Endpoint successfully created");
                    }
                    catch (std::exception& e)
                    {
                        std::cerr << "TCP Exception: " << e.what() << "\n";
                    }
                }
            }
            else{
                command->getSender()->deliverAnswer("Cannot understand command Argument "+command->getArguments()+" for Command "+command->getCommand()+"\n");
            }
        }
        else{
            command->getSender()->deliverAnswer("Cannot understand command "+command->getCommand()+"\n");
        }
        found=true;
    }
    if(!found){
        command->getSender()->deliverAnswer("The receiver "+command->getReceiver()+" was not found \n");
    }
}

void NMEAServer::run(){
    shouldRun=true;
    while(shouldRun){
        boost::mutex::scoped_lock lock(msgsMutex);
        msgsCond.wait(lock);
        while(!msgs.empty()){
            NMEAmsg_ptr tmp_msg = msgs.front();
            std::cout << (*tmp_msg).to_str(true);
            for (std::list<NMEAEndpoint_ptr>::const_iterator endpoint = endpoints.begin(), end = endpoints.end(); endpoint != end; ++endpoint) {
                (*endpoint)->deliver(msgs.front());
            }
            msgs.pop();
        }
        msgsMutex.unlock();
    }
}

void NMEAServer::stop(){
    shouldRun=false;
    msgsCond.notify_all();
}
