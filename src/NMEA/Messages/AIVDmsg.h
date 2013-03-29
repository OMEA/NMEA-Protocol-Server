//
//  AIVDmsg.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 27.03.13.
//
//

#ifndef __NMEA_Protocol_Server__AIVDmsg__
#define __NMEA_Protocol_Server__AIVDmsg__

#include <iostream>
#include "../NMEAmsg.h"
#include "navobjects.h"

class AIVDmsg: public NMEAmsg {
public:
    AIVDmsg(std::string parseMsg, Endpoint_ptr sender, bool check_checksum=false);
    AIVDmsg(Endpoint_ptr sender, bool own);
    
    void setFragmentCount(unsigned int fragmentCount){this->fragmentCount=fragmentCount;}
    unsigned int getFragmentCount(){return fragmentCount;}
    
    void setFragment(unsigned int fragment){this->fragment=fragment;}
    unsigned int getFragment(){return fragment;}
    
    void setMessageId(unsigned int messageId){this->messageId=messageId;}
    unsigned int getMessageId(){return messageId;}
    
    void setChannelCode(char channelCode){this->channelCode=channelCode;}
    char getChannelCode(){return channelCode;}
    
    void setPayload(std::string payload){this->payload=payload;}
    std::string getPayload(){return payload;}
    
    void setFillBits(unsigned char fillBits){this->fillBits=fillBits;}
    unsigned char getFillBits(){return fillBits;}
    
    virtual void setMsg(std::string msg);
    virtual const std::string getMsg()const;
private:
    unsigned int fragmentCount;
    unsigned int fragment;
    unsigned int messageId;
    char channelCode;
    std::string payload;
    unsigned char fillBits;
};

typedef boost::shared_ptr<AIVDmsg> AIVDmsg_ptr;

#endif /* defined(__NMEA_Protocol_Server__AIVDmsg__) */
