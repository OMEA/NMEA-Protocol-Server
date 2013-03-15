//
//  Command.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 08.03.13.
//
//

#include "Command.h"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::algorithm;

Command::Command(std::string parseCommand, Endpoint_ptr sender): Message(sender){
    boost::regex reg("^#([^#]*)#([^ ]+)(\\s[[:print:]]+)?\\h?$");
    boost::regex regLocal("^#([^ ]+)(\\s[[:print:]]+)?\\h?$");
    boost::cmatch matches;
    
    if(boost::regex_search(parseCommand.c_str(), matches, reg)){
        setReceiver(std::string(matches[1].first, matches[1].second));
        if(getReceiver()==""){
            setReceiver("server");
        }
        command=std::string(matches[2].first, matches[2].second);
        arguments=std::string(matches[3].first, matches[3].second);
    }
    else if(boost::regex_search(parseCommand.c_str(), matches, regLocal)){
        setReceiver("local");
        command=std::string(matches[1].first, matches[1].second);
        arguments=std::string(matches[2].first, matches[2].second);
    }
    else{
        throw std::invalid_argument("No Command, could not parse!");
    }
    trim(command);
    trim(arguments);
}

const std::string Command::to_str() const {
    std::stringstream ss;
    std::stringstream ss2;
    if(getReceiver().length()>0){
        ss << '#' << getReceiver();
    }
    ss << '#' << command << ' ' << arguments;
    return ss.str();
}


void Command::answer(std::string message, Endpoint_ptr sender){
    Answer_ptr answer(new Answer(message, sender));
    CommandEndpoint_ptr cmd_endpoint = boost::dynamic_pointer_cast<CommandEndpoint>(this->getSender());
    cmd_endpoint->deliver(answer);
}