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
    Answer(std::string parseMessage, Endpoint_ptr sender);
    virtual ~Answer(){}
    
    void setMessage(std::string message){this->message = message;}
    const std::string getMessage()const{return this->message;}
    
    const std::string to_str() const;
    
    private:
    std::string message;
};

typedef boost::shared_ptr<Answer> Answer_ptr;


#endif /* defined(__NMEA_Protocol_Server__Answer__) */
