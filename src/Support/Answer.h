//
//  Answer.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 15.03.13.
//
//

#ifndef __NMEA_Protocol_Server__Answer__
#define __NMEA_Protocol_Server__Answer__

#include <iostream>


#include "Message.h"
#include "Endpoint.h"



class Answer: public Message
{
public:
    enum Type {SUCCESS=0, UNSPEC_ERROR, UNKNOWN_RECEIVER, UNKNOWN_CMD, WRONG_ARGS};
public:
    Answer(Answer::Type type, std::string parseMessage, Endpoint_ptr sender);
    virtual ~Answer(){}
    
    void setMessage(std::string message){this->message = message;}
    const std::string getMessage()const{return this->message;}
    void setType(Answer::Type type){this->type = type;}
    const Answer::Type getType()const{return this->type;}
    
    const std::string to_str() const;
    
    private:
    std::string message;
    Type type;
};

typedef boost::shared_ptr<Answer> Answer_ptr;


#endif /* defined(__NMEA_Protocol_Server__Answer__) */
