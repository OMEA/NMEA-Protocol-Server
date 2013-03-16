//
//  Message.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 14.03.13.
//
//

#include "Message.h"


Message::Message(Endpoint_ptr sender){
    this->sender=sender;
    received = boost::posix_time::microsec_clock::local_time();
}

Message::Message(){
    received = boost::posix_time::microsec_clock::local_time();
}

int Message::toBuffer(char* buffer, unsigned int size){
    //todo checksum
    std::string tmpString = to_str();
    unsigned int length = tmpString.length();
    if(length>size)
        return -1;
    memcpy(buffer,tmpString.c_str(),length);
    return length;
}

std::ostream& operator<< (std::ostream &out, Message &message)
{
    out << message.to_str();
    return out;
}