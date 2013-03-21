//
//  NMEAmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 22.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "NMEAmsg.h"
#include "Messages/RMCmsg.h"

#include <boost/regex.hpp>
#include <string>

boost::shared_ptr<NMEAmsg> NMEAmsg::factory(std::string parseMsg, Endpoint_ptr sender) {
    boost::shared_ptr<NMEAmsg> newMsg;
    std::string id=parseMsg.substr(1, parseMsg.find_first_of(',',2)-1);
    if(id=="GPRMC"){
        newMsg = boost::shared_ptr<RMCmsg>(new RMCmsg(parseMsg, sender));
    }
    else{
        newMsg = boost::shared_ptr<NMEAmsg>(new NMEAmsg(parseMsg, sender));
    }
    return newMsg;
}

NMEAmsg::NMEAmsg(): Message(){
    //TODO
    start='$'; id="P"; msg="none";
}

NMEAmsg::NMEAmsg(std::string parseMsg, Endpoint_ptr sender): Message(sender){
    boost::regex reg("^([!\\$])([[:alnum:]]{1,5}),([[:print:]]+)\\h?$");
    boost::regex regChecksum("^([!\\$])([[:alnum:]]{1,5}),([[:print:]]+)\\*([[:xdigit:]]{1,2})\\h?$");
    boost::cmatch matches;
    bool found=false;
    
    if(boost::regex_search(parseMsg.c_str(), matches, regChecksum)){
        found=true;
    }
    else if(boost::regex_search(parseMsg.c_str(), matches, reg)){
        found=true;
    }
    if(found){
        start=*matches[1].first;
        id=std::string(matches[2].first, matches[2].second);
        msg=std::string(matches[3].first, matches[3].second);
    }
    else{
        throw std::invalid_argument("No NMEA-0183 Message, could not parse!");
    }
}

std::size_t NMEAmsg::hash_value(){
    boost::hash<std::string> hasher;
    return hasher(id+msg);
}

const char * NMEAmsg::data(bool checksum) const{
    //todo checksum
    return to_str().c_str();
}

void NMEAmsg::setMsg(std::string msg){
    msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
    msg.erase(std::remove(msg.begin(), msg.end(), '\r'), msg.end());
    this->msg = msg;
}

const size_t NMEAmsg::length(bool checksum) const{
    //TODO checksum
    return to_str().length();
}

const std::string NMEAmsg::to_str() const {
    //TODO
    bool checksum = true;
    std::ostringstream ss1;
    std::ostringstream ss2;
    ss2 << this->id << "," << this->msg;
    if(checksum){
        std::string tmpString = ss2.str();
        char checksumchar = tmpString.at(0);
        for(unsigned int i = 1; i< tmpString.length();i++){
            checksumchar = checksumchar^tmpString.at(i);
        }
        char checksumhex[3];
        sprintf(checksumhex, "%02x", checksumchar);
        ss2 << '*' << checksumhex;
    }
    ss1 << this->start << ss2.str() << '\r' << '\n';
    return ss1.str();
}

std::ostream& operator<<(std::ostream &strm, const NMEAmsg &msg) {
    return strm << msg.to_str();
}