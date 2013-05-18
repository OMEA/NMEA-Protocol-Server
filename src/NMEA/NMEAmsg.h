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
    static boost::shared_ptr<NMEAmsg> factory(std::string parseMsg, Endpoint_ptr sender, bool check_checksum=false);
protected:
    NMEAmsg(Endpoint_ptr sender, char start, std::string id);
    NMEAmsg(std::string parseMsg, Endpoint_ptr sender, bool check_checksum=false);
public:
    virtual ~NMEAmsg(){}
    
    void setStart(char start){this->start = start;}
    const char getStart()const{return this->start;}
    void setId(std::string id){this->id = id;}
    const std::string getId()const{return this->id;}
    virtual void setMsg(std::string msg);
    virtual const std::string getMsg()const{return this->msg;}
    
    virtual const std::string to_str() const;
        
    const char *data(bool checksum=false) const;
    const size_t length(bool checksum=false) const;
    
    std::size_t hash_value();
private:
    char start;
    std::string id;
    std::string msg;
};

typedef boost::shared_ptr<NMEAmsg> NMEAmsg_ptr;


#endif /* defined(__NMEA_Protocol_Server__NMEAmsg__) */
