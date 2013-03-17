//
//  Command.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 08.03.13.
//
//

#ifndef __NMEA_Protocol_Server__Command__
#define __NMEA_Protocol_Server__Command__

#include <iostream>

#include "Message.h"
#include "Answer.h"
#include "CommandEndpoint.h"

class CommandEndpoint;
typedef boost::shared_ptr<CommandEndpoint> CommandEndpoint_ptr;

class Command: public Message, public boost::enable_shared_from_this<Command>
{
public:
    Command(std::string parseCommand, Endpoint_ptr sender);
    virtual ~Command(){}
    
    void setCommand(std::string command){this->command = command;}
    const std::string getCommand()const{return this->command;}
    void setArguments(std::string arguments){this->arguments = arguments;}
    const std::string getArguments()const{return this->arguments;}
    
    void answer(std::string message, Endpoint_ptr sender);
    void answer(Answer::Type type, std::string message, Endpoint_ptr sender);
    
    const std::string to_str() const;
private:
    std::string command;
    std::string arguments;
};

typedef boost::shared_ptr<Command> Command_ptr;


#endif /* defined(__NMEA_Protocol_Server__Command__) */
