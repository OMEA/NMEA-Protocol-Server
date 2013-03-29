//
//  NMEAmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 22.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "NMEAmsg.h"
#include "Messages/RMCmsg.h"
#include "Messages/AIVDmsg.h"

#include <boost/regex.hpp>
#include <string>
#include <algorithm>
#include <locale>

boost::shared_ptr<NMEAmsg> NMEAmsg::factory(std::string parseMsg, Endpoint_ptr sender, bool check_checksum) {
    boost::shared_ptr<NMEAmsg> newMsg;
    std::string id=parseMsg.substr(1, parseMsg.find_first_of(',',2)-1);
    if(id=="GPRMC"){
        newMsg = boost::shared_ptr<RMCmsg>(new RMCmsg(parseMsg, sender, check_checksum));
    }
    else if(id=="AIVDO" || id=="AIVDM"){
        newMsg = boost::shared_ptr<AIVDmsg>(new AIVDmsg(parseMsg, sender, check_checksum));
    }
    else{
        newMsg = boost::shared_ptr<NMEAmsg>(new NMEAmsg(parseMsg, sender, check_checksum));
    }
    return newMsg;
}

NMEAmsg::NMEAmsg(Endpoint_ptr sender, char start, std::string id): Message(sender){
    //TODO
    this->start=start;
    this->id=id;
    this->msg="none";
}

NMEAmsg::NMEAmsg(std::string parseMsg, Endpoint_ptr sender, bool check_checksum): Message(sender){
    boost::regex reg("^([!\\$])([[:alnum:]]{1,5}),([^\\*]+)(\\*?([[:xdigit:]]{2})?)\\h?$");
    boost::cmatch matches;
    
    if(boost::regex_search(parseMsg.c_str(), matches, reg)){
        start=*matches[1].first;
        id=std::string(matches[2].first, matches[2].second);
        msg=std::string(matches[3].first, matches[3].second);
        std::string msgChecksum = matches[5];
        if(check_checksum && msgChecksum.length()==2){
            std::string tmpString = std::string(matches[2].first, matches[3].second);
            char checksumchar = tmpString.at(0);
            for(unsigned int i = 1; i< tmpString.length();i++){
                checksumchar = checksumchar^tmpString.at(i);
            }
            char checksumhex[3];
            sprintf(checksumhex, "%02X", checksumchar);
            std::string calcChecksum = std::string(checksumhex);
            //std::transform(calcChecksum.begin(), calcChecksum.end(), calcChecksum.begin(), std::toupper);
            msgChecksum = parseMsg.substr(parseMsg.find_first_of('*',2)+1);
            if(calcChecksum!=msgChecksum){
                throw std::invalid_argument("Wrong checksum!, should be "+calcChecksum+" instead of "+msgChecksum);
            }
        }
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
    ss2 << this->id << "," << getMsg();
    if(checksum){
        std::string tmpString = ss2.str();
        char checksumchar = tmpString.at(0);
        for(unsigned int i = 1; i< tmpString.length();i++){
            checksumchar = checksumchar^tmpString.at(i);
        }
        char checksumhex[3];
        sprintf(checksumhex, "%02X", checksumchar);
        ss2 << '*' << checksumhex;
    }
    ss1 << this->start << ss2.str() << '\r' << '\n';
    return ss1.str();
}

std::ostream& operator<<(std::ostream &strm, const NMEAmsg &msg) {
    return strm << msg.to_str();
}