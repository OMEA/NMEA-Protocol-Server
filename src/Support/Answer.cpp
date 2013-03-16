//
//  Answer.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 15.03.13.
//
//

#include "Answer.h"

Answer::Answer(Answer::Type type, std::string parseMessage, Endpoint_ptr sender): Message(sender), type(type){
    trim(parseMessage);
    setMessage(parseMessage);
}

const std::string Answer::to_str() const {
    std::stringstream ss;
    ss << getSender()->getId()<<':'<<getMessage() << '\r' << '\n';
    return ss.str();
}
