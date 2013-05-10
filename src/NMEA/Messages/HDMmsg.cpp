//
//  HDMmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 10.05.13.
//
//

#include "HDMmsg.h"
#include <boost/regex.hpp>

#define MATCH_HEADING_MAGNETIC 1

HDMmsg::HDMmsg(std::string parseMsg, Endpoint_ptr sender, bool check_checksum): NMEAmsg(parseMsg, sender, check_checksum){
    setMsg(NMEAmsg::getMsg());
}

void HDMmsg::setMsg(std::string msg){
    boost::regex reg("^([^,]*),M$");
    boost::cmatch matches;
    if(boost::regex_search(msg.c_str(), matches, reg)){
        try{
            headingMagnetic = boost::lexical_cast<float>(matches[MATCH_HEADING_MAGNETIC]);
        }
        catch(const boost::bad_lexical_cast&){
            headingMagnetic = 0;
        }
    }
    else{
        throw std::invalid_argument("Cannot parse HDM message!");
    }
}
const std::string HDMmsg::getMsg()const{
    std::ostringstream oss;
    return oss.str();
}

HDMmsg::HDMmsg(Endpoint_ptr sender): NMEAmsg(sender,'$',"HDM"){
    
}