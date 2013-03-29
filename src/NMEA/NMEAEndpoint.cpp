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
#include <boost/regex.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <limits>

NMEAEndpoint::NMEAEndpoint(boost::shared_ptr<Endpoint> connectedTo): CommandEndpoint(connectedTo){}

NMEAEndpoint::NMEAEndpoint(){}

void NMEAEndpoint::initialize(){
    CommandEndpoint::initialize();
    registerBoolCmd("input","Input mode", "When on, messages are forwarded to the Server, otherwise messages are discarded.",  &input, true, true);
    registerBoolCmd("output", "Output mode", "When on, messages comming from the Server are sent to the remote end, otherwise messages are discarded.", &output, true, true);
    registerBoolCmd("mirror", "Mirror mode", "When on, messages comming from the Server are sent to the remote end even if they were originally coming from there", &portmirror, false, true);
    registerBoolCmd("checksum", "Print checksums", "When on, messages are sent with their checksum (*XX)", &checksum, true, true);
    registerBoolCmd("force_checksum", "Force checksum check", "When on, messages are checked against their checksum (*XX)", &check_checksum, false, true);
    registerIntCmd("incompress","Input Compressor Number", "Defines the maximum number of consecutive incoming messages that are compressed. 0 = no compresson; -1 = infinite compression", &incompress_messages, 0, -1, std::numeric_limits<int>::max(), true);
    registerIntCmd("outcompress","Output Compressor Number", "Defines the maximum number of consecutive outgoing messages that are compressed. 0 = no compresson; -1 = infinite compression", &outcompress_messages, 0, -1, std::numeric_limits<int>::max(), true);
    
    registerStringVectorCmd("in_black","Incoming Blacklist", "Define Message-Ids of incoming messages that should be dropped. You can use * for generalization (e.g. GS* to match GSV and GSL).",  &in_black, true);
    registerStringVectorCmd("in_white","Incoming Whitelist", "Define Message-Ids of incoming messages that should never be dropped. You can use * for generalization (e.g. GS* to match GSV and GSL).",  &in_white, true);
    registerStringVectorCmd("out_black","Outgoing Blacklist", "Define Message-Ids of outgoing messages that should be dropped. You can use * for generalization (e.g. GS* to match GSV and GSL).",  &out_black, true);
    registerStringVectorCmd("out_white","Outgoing Whitelist", "Define Message-Ids of outgoing messages that should never be dropped. You can use * for generalization (e.g. GS* to match GSV and GSL).",  &out_white, true);
    registerBoolCmd("stats", "Statistics", "When on, stats are calculated", &stats_enabled, false, true);
    //boost::function<void (Command_ptr)> func = boost::bind(&NMEAEndpoint::add_midpoint_cmd, this, _1);
    //registerVoidCmd("add_midpoint","Midpoint hinzufügen", "Add a midpoint and connect it between this endpoint and the Endpoint it is connected to. Midpoint type and arguments must be passed.",  func);
    stat_list_size=60;
    in_total_size=0;
    out_total_size=0;
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
    if(output && (portmirror || msg->getSender()!=this->v_shared_from_this()) && black_and_white(msg, &out_black, &out_white) && !compress(msg, &outcompressor_hashes,outcompress_messages)){
        if(stats_enabled){
            update_stats_deliver(msg);
        }
        deliver_impl(msg);
    }
}

void NMEAEndpoint::receive(NMEAmsg_ptr msg){
    if(input && black_and_white(msg, &in_black, &in_white) && !compress(msg, &incompressor_hashes,incompress_messages)){
        if(stats_enabled){
            update_stats_receive(msg);
        }
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
        if(msghash == hash && (num_messages<0 || prev_messages < (unsigned int)num_messages)){
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
    for(std::vector<std::string>::const_iterator stringId = whitelist->begin(); stringId != whitelist->end();++stringId){
        std::string stringId_cpy(*stringId);
        boost::replace_all(stringId_cpy, "*", "(.*)");
        boost::regex reg("^"+stringId_cpy+"$");
        boost::cmatch matches;
        if(boost::regex_search(msg->getId().c_str(), matches, reg)){
            return true;
        }
    }
    for(std::vector<std::string>::const_iterator stringId = blacklist->begin(); stringId != blacklist->end();++stringId){
        std::string stringId_cpy(*stringId);
        boost::replace_all(stringId_cpy, "*", "(.*)");
        boost::regex reg("^"+stringId_cpy+"$");
        boost::cmatch matches;
        if(boost::regex_search(msg->getId().c_str(), matches, reg)){
            return false;
        }
    }
    return true;
}

void NMEAEndpoint::update_stats_deliver(NMEAmsg_ptr msg){
    if(out_stat_list.size()>=stat_list_size){
        std::pair<boost::posix_time::ptime, size_t> front = out_stat_list.front();
        out_total_size-=front.second;
        out_stat_list.pop_front();
    }
    out_total_size+=msg->length();
    out_stat_list.push_back(std::pair<boost::posix_time::ptime, size_t>(msg->getReceived(), msg->length()));
}

void NMEAEndpoint::update_stats_receive(NMEAmsg_ptr msg){
    if(in_stat_list.size()>=stat_list_size){
        std::pair<boost::posix_time::ptime, size_t> front = in_stat_list.front();
        in_total_size-=front.second;
        in_stat_list.pop_front();
    }
    in_total_size+=msg->length();
    in_stat_list.push_back(std::pair<boost::posix_time::ptime, size_t>(msg->getReceived(), msg->length()));
}
