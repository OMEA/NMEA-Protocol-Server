//
//  AIVDmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 27.03.13.
//
//

#include "AIVDmsg.h"
#include <boost/regex.hpp>

#define MATCH_FRAGMENT_COUNT 1
#define MATCH_FRAGMENT MATCH_FRAGMENT_COUNT+1
#define MATCH_MESSAGE_ID MATCH_FRAGMENT+1
#define MATCH_CHANNEL_CODE MATCH_MESSAGE_ID+1
#define MATCH_PAYLOAD MATCH_CHANNEL_CODE+1
#define MATCH_FILL_BITS MATCH_PAYLOAD+1

unsigned int fragmentCount;
unsigned int fragment;
unsigned int messageId;
char channelCode;
std::string payload;
unsigned char fillBits;

AIVDmsg::AIVDmsg(std::string parseMsg, Endpoint_ptr sender): NMEAmsg(parseMsg, sender){
    setMsg(NMEAmsg::getMsg());
}

void AIVDmsg::setMsg(std::string msg){
    boost::regex reg("^(\\d+),(\\d+),(\\d*),([AB]),([^,]*),(\\d)$");
    boost::cmatch matches;
    if(boost::regex_search(msg.c_str(), matches, reg)){
        try{
            fragmentCount = boost::lexical_cast<unsigned int>(matches[MATCH_FRAGMENT_COUNT]);
        }
        catch(const boost::bad_lexical_cast&){
            fragmentCount = 0;
        }
        try{
            fragment = boost::lexical_cast<unsigned int>(matches[MATCH_FRAGMENT]);
        }
        catch(const boost::bad_lexical_cast&){
            fragment = 0;
        }
        try{
            messageId = boost::lexical_cast<unsigned int>(matches[MATCH_MESSAGE_ID]);
        }
        catch(const boost::bad_lexical_cast&){
            messageId=0;
        }
        channelCode = *matches[MATCH_CHANNEL_CODE].first;
        payload = matches[MATCH_PAYLOAD];
        try{
            fillBits = boost::lexical_cast<unsigned int>(matches[MATCH_FILL_BITS]);
        }
        catch(const boost::bad_lexical_cast&){
            fillBits=0;
        }
    }
    else{
        throw std::invalid_argument("Cannot parse AIVD* message!");
    }
}
const std::string AIVDmsg::getMsg()const{
    std::ostringstream oss;
    std::string messageId = "";
    if(this->messageId>0){
        messageId= boost::lexical_cast<std::string>(this->messageId);
    }
    oss << this->fragmentCount << "," << this->fragment << "," << messageId << "," << this->channelCode << "," << this->payload  << "," << (unsigned int)this->fillBits;
    return oss.str();
}

AIVDmsg::AIVDmsg(Endpoint_ptr sender, bool own): NMEAmsg(sender,'!', (own)?"AIVDO":"AIVDM"){
    
}