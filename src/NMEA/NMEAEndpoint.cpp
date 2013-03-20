//
//  NMEAEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "NMEAEndpoint.h"
#include "NMEAMidpoint.h"
#include <boost/bind.hpp>
#include <limits>

NMEAEndpoint::NMEAEndpoint(boost::shared_ptr<Endpoint> connectedTo): CommandEndpoint(connectedTo){}

NMEAEndpoint::NMEAEndpoint(){}

void NMEAEndpoint::initialize(){
    CommandEndpoint::initialize();
    registerBoolCmd("input","Input mode", "When on, messages are forwarded to the Server, otherwise messages are discarded.",  &input, true, true);
    registerBoolCmd("output", "Output mode", "When on, messages comming from the Server are sent to the remote end, otherwise messages are discarded.", &output, true, true);
    registerBoolCmd("mirror", "Mirror mode", "When on, messages comming from the Server are sent to the remote end even if they were originally coming from there", &portmirror, false, true);
    registerBoolCmd("checksum", "Print checksums", "When on, messages are sent with their checksum (*XX)", &checksum, true, true);
    registerIntCmd("incompress","Input Compressor Number", "Defines the maximum number of consecutive incoming messages that are compressed. 0 = no compresson; -1 = infinite compression", &incompress_messages, 0, -1, std::numeric_limits<int>::max(), true);
    registerIntCmd("outcompress","Output Compressor Number", "Defines the maximum number of consecutive outgoing messages that are compressed. 0 = no compresson; -1 = infinite compression", &outcompress_messages, 0, -1, std::numeric_limits<int>::max(), true);
    
    registerStringVectorCmd("in_black","Incoming Blacklist", "Define Message-Ids of incoming messages that should be dropped. You can use * for generalization (e.g. GS* to match GSV and GSL).",  &in_black, true);
    registerStringVectorCmd("in_white","Incoming Whitelist", "Define Message-Ids of incoming messages that should never be dropped. You can use * for generalization (e.g. GS* to match GSV and GSL).",  &in_white, true);
    registerStringVectorCmd("out_black","Outgoing Blacklist", "Define Message-Ids of outgoing messages that should be dropped. You can use * for generalization (e.g. GS* to match GSV and GSL).",  &out_black, true);
    registerStringVectorCmd("out_white","Outgoing Whitelist", "Define Message-Ids of outgoing messages that should never be dropped. You can use * for generalization (e.g. GS* to match GSV and GSL).",  &out_white, true);
    //boost::function<void (Command_ptr)> func = boost::bind(&NMEAEndpoint::add_midpoint_cmd, this, _1);
    //registerVoidCmd("add_midpoint","Midpoint hinzufügen", "Add a midpoint and connect it between this endpoint and the Endpoint it is connected to. Midpoint type and arguments must be passed.",  func);
}

void NMEAEndpoint::add_midpoint_cmd(Command_ptr command){
    try{
        NMEAMidpoint_ptr midpoint = NMEAMidpoint::factory(getConnectedTo(),command->getArguments());
        unregisterEndpoint();
        setConnectedTo(midpoint);
        registerEndpoint();
        
        command->answer("Midpoint created and connected.", v_shared_from_this());
    }
    catch(std::exception& e){
        command->answer(Answer::WRONG_ARGS, "Cannot understand "+command->getArguments()+" for command "+command->getCommand()+". Not a midpoint class\n", v_shared_from_this());
    }
}

void NMEAEndpoint::deliver(NMEAmsg_ptr msg){
    if(output && (portmirror || msg->getSender()!=this->v_shared_from_this()) && black_and_white(msg, &out_black, &out_white) && !compress(msg, &outcompressor_hashes,outcompress_messages))
        deliver_impl(msg);
}

void NMEAEndpoint::receive(NMEAmsg_ptr msg){
    if(input && black_and_white(msg, &in_black, &in_white) && !compress(msg, &incompressor_hashes,incompress_messages)){
        receive_impl(msg);
    }
}

void NMEAEndpoint::receive_impl(NMEAmsg_ptr msg){
    msg->setSender(this->v_shared_from_this());
    NMEAEndpoint_ptr nmeaEndpoint = boost::dynamic_pointer_cast<NMEAEndpoint>(getConnectedTo());
    if(nmeaEndpoint){
        nmeaEndpoint->receive(msg);
    }
    else{
        std::cerr << "Cannot send NMEA to none NMEAEndpoint" << std::endl;
    }
}

void NMEAEndpoint::receive(Command_ptr command){
    CommandEndpoint::receive(command);
}

bool NMEAEndpoint::compress(NMEAmsg_ptr msg, std::map<std::string,std::pair<std::size_t, unsigned int> > *hashes, int num_messages){
    if(num_messages==0){
        return false;
    }
    std::size_t hash = msg->hash_value();
    if(hashes->find(msg->getId())!=hashes->end()){
        std::pair<std::size_t, unsigned int> entry = (*(hashes->find(msg->getId()))).second;
        std::size_t msghash= entry.first;
        unsigned int prev_messages = entry.second;
        if(msghash == hash && (prev_messages < num_messages || num_messages==-1)){
            prev_messages++;
            hashes->erase(msg->getId());
            hashes->insert(std::pair<std::string, std::pair<std::size_t, unsigned int> >(msg->getId(),std::pair<std::size_t, unsigned int>(msghash, prev_messages)));
            return true;
        }
    }
    hashes->erase(msg->getId());
    hashes->insert(std::pair<std::string, std::pair<std::size_t, unsigned int> >(msg->getId(),std::pair<std::size_t, unsigned int>(hash, 0)));
    NMEAEndpoint_ptr nmeaEndpoint = boost::dynamic_pointer_cast<NMEAEndpoint>(getConnectedTo());
    return false;
}

bool NMEAEndpoint::black_and_white(NMEAmsg_ptr msg, const std::vector<std::string> *blacklist, const std::vector<std::string> *whitelist){
    if(std::find(whitelist->begin(), whitelist->end(), msg->getId()) != whitelist->end()){
        return true;
    }
    else if(std::find(blacklist->begin(), blacklist->end(), msg->getId()) != blacklist->end()){
        return false;
    }
    return true;
}