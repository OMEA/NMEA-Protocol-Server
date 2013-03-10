//
//  NMEAmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 22.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#include "NMEAmsg.h"

#include <boost/regex.hpp>
#include <string>

NMEAmsg::NMEAmsg(){
    received = boost::posix_time::microsec_clock::local_time();
    
    //TODO
     start='$'; id="P"; msg="none";
}

NMEAmsg::NMEAmsg(std::string parseMsg, NMEAEndpoint_ptr sender){
    received = boost::posix_time::microsec_clock::local_time();
    this->sender=sender;
    
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

const char * NMEAmsg::data(bool checksum) const{
    return to_str(checksum).c_str();
}

void NMEAmsg::setMsg(std::string msg){
    msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
    msg.erase(std::remove(msg.begin(), msg.end(), '\r'), msg.end());
    this->msg = msg;
}

int NMEAmsg::toBuffer(char* buffer, unsigned int size, bool checksum){
    std::string tmpString = to_str(checksum);
    unsigned int length = tmpString.length();
    if(length>size)
        return -1;
    memcpy(buffer,tmpString.c_str(),length);
    return length;
}

const size_t NMEAmsg::length(bool checksum) const{
    return to_str(checksum).length();
}

const std::string NMEAmsg::to_str(bool checksum) const {
    std::stringstream ss1;
    std::stringstream ss2;
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
    ss1 << this->start << ss2.str() << '\n';
    return ss1.str();
}

std::ostream& operator<<(std::ostream &strm, const NMEAmsg &msg) {
    return strm << msg.to_str();
}