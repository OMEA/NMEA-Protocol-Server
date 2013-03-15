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

std::ostream& operator<< (std::ostream &out, Message &message)
{
    out << message.to_str();
    return out;
}