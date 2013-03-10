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
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class NMEAEndpoint;
typedef boost::shared_ptr<NMEAEndpoint> NMEAEndpoint_ptr;

class Command
{
public:
    Command(std::string parseCommand, NMEAEndpoint_ptr sender);
    
    void setReceived(boost::posix_time::ptime received){this->received = received;}
    const boost::posix_time::ptime getReceived()const{return this->received;}
    void setSender(NMEAEndpoint_ptr sender){this->sender = sender;}
    const NMEAEndpoint_ptr getSender()const{return this->sender;}
    void setReceiver(std::string receiver){this->receiver = receiver;}
    const std::string getReceiver()const{return this->receiver;}
    void setCommand(std::string receiver){this->command = command;}
    const std::string getCommand()const{return this->command;}
    void setArguments(std::string arguments){this->arguments = arguments;}
    const std::string getArguments()const{return this->arguments;}
    
    const std::string to_str() const;
private:
    boost::posix_time::ptime received;
    NMEAEndpoint_ptr sender;
    std::string receiver;
    std::string command;
    std::string arguments;
};

typedef boost::shared_ptr<Command> Command_ptr;


#endif /* defined(__NMEA_Protocol_Server__Command__) */
