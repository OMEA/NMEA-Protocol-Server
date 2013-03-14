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

class NMEAEndpoint;
typedef boost::shared_ptr<NMEAEndpoint> NMEAEndpoint_ptr;

using namespace boost::algorithm;

class Message
{
public:
    Message(NMEAEndpoint_ptr sender);
    Message();
    
    void setReceived(boost::posix_time::ptime received){this->received = received;}
    const boost::posix_time::ptime getReceived()const{return this->received;}
    void setSender(NMEAEndpoint_ptr sender){this->sender = sender;}
    const NMEAEndpoint_ptr getSender()const{return this->sender;}
    void setReceiver(std::string receiver){trim(receiver); this->receiver = receiver;}
    const std::string getReceiver()const{return this->receiver;}
    
    virtual const std::string to_str() const = 0;
private:
    boost::posix_time::ptime received;
    NMEAEndpoint_ptr sender;
    std::string receiver;
};

typedef boost::shared_ptr<Message> Message_ptr;


#endif /* defined(__NMEA_Protocol_Server__Message__) */
