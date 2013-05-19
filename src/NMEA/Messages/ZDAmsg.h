//
//  ZDAmsg.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 10.05.13.
//
//

#ifndef __NMEA_Protocol_Server__ZDAmsg__
#define __NMEA_Protocol_Server__ZDAmsg__

#include <iostream>
#include "../NMEAmsg.h"
#include "navobjects.h"
#include <boost/date_time/local_time/local_time.hpp> 

class ZDAmsg: public NMEAmsg {
public:
    ZDAmsg(std::string parseMsg, Endpoint_ptr sender, bool check_checksum=false);
    ZDAmsg(Endpoint_ptr sender);
    
    void setTimeDate(boost::posix_time::ptime time_date){this->time_date=time_date;}
    const boost::posix_time::ptime getTimeDate()const {return time_date;}
    
    void setTimezone(boost::local_time::posix_time_zone zone){this->zone=zone;}
    const boost::local_time::posix_time_zone getTimezone()const{return zone;}
    
    virtual void setMsg(std::string msg);
    virtual const std::string getMsg()const;
private:
    boost::posix_time::ptime time_date;
    boost::local_time::posix_time_zone zone;
};

typedef boost::shared_ptr<ZDAmsg> ZDAmsg_ptr;

#endif /* defined(__NMEA_Protocol_Server__ZDAmsg__) */
