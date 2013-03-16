//
//  NMEAmsg.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 22.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#ifndef __NMEA_Protocol_Server__NMEAmsg__
#define __NMEA_Protocol_Server__NMEAmsg__

#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <boost/shared_ptr.hpp>

#include "../Support/Message.h"

class NMEAEndpoint;
typedef boost::shared_ptr<NMEAEndpoint> NMEAEndpoint_ptr;

class NMEAmsg: public Message {
public:
    NMEAmsg();
    NMEAmsg(std::string parseMsg, Endpoint_ptr sender);
    virtual ~NMEAmsg(){}
    
    void setStart(char start){this->start = start;}
    const char getStart()const{return this->start;}
    void setId(char id){this->id = id;}
    const std::string getId()const{return this->id;}
    void setMsg(std::string msg);
    const std::string getMsg()const{return this->msg;}
    
    virtual const std::string to_str() const;
        
    const char *data(bool checksum=false) const;
    const size_t length(bool checksum=false) const;
    
private:
    char start;
    std::string id;
    std::string msg;
};

typedef boost::shared_ptr<NMEAmsg> NMEAmsg_ptr;


#endif /* defined(__NMEA_Protocol_Server__NMEAmsg__) */
