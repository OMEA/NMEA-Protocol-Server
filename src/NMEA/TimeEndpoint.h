//
//  TimeEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 10.05.13.
//
//

#ifndef __NMEA_Protocol_Server__TimeEndpoint__
#define __NMEA_Protocol_Server__TimeEndpoint__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/local_time/local_time.hpp> 
#include "NMEAEndpoint.h"

class TimeEndpoint: public NMEAEndpoint, public boost::enable_shared_from_this<TimeEndpoint>{
public:
    static boost::shared_ptr<TimeEndpoint> factory(boost::shared_ptr<Endpoint> connectedTo);
protected:
    TimeEndpoint(boost::shared_ptr<Endpoint> connectedTo);
    virtual void initialize();
public:
    std::string getId(){return "TimeEndpoint";}
protected:
    virtual void deliver_impl(NMEAmsg_ptr msg);
    virtual void deliverAnswer_impl(Answer_ptr answer){};
    virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
    
private:
    void list_cmd(Command_ptr command);
    void set_zone_cmd(Command_ptr command);
    void get_zone_cmd(Command_ptr command);
public:
    void setTime(boost::posix_time::ptime time){this->time=time;}
    boost::posix_time::ptime getTime(){return time;}
    
    void setTimezone(boost::local_time::posix_time_zone zone){this->zone=zone;}
    boost::local_time::posix_time_zone getTimezone(){return zone;}
private:
    bool gen_zda;
    bool correction;
    bool receivedData;
    boost::posix_time::ptime time;
    boost::local_time::posix_time_zone zone;
};

typedef boost::shared_ptr<TimeEndpoint> TimeEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__TimeEndpoint__) */
