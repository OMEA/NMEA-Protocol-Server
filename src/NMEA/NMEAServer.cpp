//
//  Server.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 22.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "NMEAServer.h"

NMEAServer* NMEAServer::getInstance() {
    static NMEAServer theInstance;
    return &theInstance;
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
        if(command->getReceiver()==(*endpoint)->getId()){
            (*endpoint)->receiveCommand(command);
            found=true;
        }
    }
    if(command->getReceiver()=="server"){
        if(command->getCommand()=="exit"){
            command->getSender()->deliverAnswer("The exit command is not implemented\n");
            //TODO SHUTDOWN IMPLEMENTIEREN
        }
        if(command->getCommand()=="endpoints"){
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
    std::cout << "Serverthread started" << std::endl;
    while(true){
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
