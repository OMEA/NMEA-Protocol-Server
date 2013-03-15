//
//  Message.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 14.03.13.
//
//

#ifndef __NMEA_Protocol_Server__Message__
#define __NMEA_Protocol_Server__Message__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

class Endpoint;
typedef boost::shared_ptr<Endpoint> Endpoint_ptr;

using namespace boost::algorithm;

class Message
{
public:
    Message(Endpoint_ptr sender);
    Message();
    
    void setReceived(boost::posix_time::ptime received){this->received = received;}
    const boost::posix_time::ptime getReceived()const{return this->received;}
    void setSender(Endpoint_ptr sender){this->sender = sender;}
    const Endpoint_ptr getSender()const{return this->sender;}
    void setReceiver(std::string receiver){trim(receiver); this->receiver = receiver;}
    const std::string getReceiver()const{return this->receiver;}
    
    virtual const std::string to_str() const = 0;
    operator std::string ( ) const { return to_str(); }
    friend std::ostream& operator<< (std::ostream &out, Message &message);
private:
    boost::posix_time::ptime received;
    Endpoint_ptr sender;
    std::string receiver;
};

typedef boost::shared_ptr<Message> Message_ptr;


#endif /* defined(__NMEA_Protocol_Server__Message__) */
