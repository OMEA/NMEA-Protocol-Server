//
//  Answer.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 15.03.13.
//
//

#include "Answer.h"

Answer::Answer(Answer::Type type, std::string parseMessage, Endpoint_ptr sender, boost::shared_ptr<Command> origin_cmd): Message(sender), origin_cmd(origin_cmd), type(type){
    trim(parseMessage);
    setMessage(parseMessage);
}

const std::string Answer::to_str() const {
    std::ostringstream ss;
    ss << getSender()->getId()<< " ->> "<<getMessage() << '\r' << '\n';
    return ss.str();
}
