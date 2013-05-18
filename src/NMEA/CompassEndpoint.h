//
//  CompassEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 09.05.13.
//
//

#ifndef __NMEA_Protocol_Server__CompassEndpoint__
#define __NMEA_Protocol_Server__CompassEndpoint__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/process.hpp> 
#include "NMEAEndpoint.h"
#include "Messages/navobjects.h"

typedef boost::asio::posix::stream_descriptor pipe_end;

class CompassEndpoint: public NMEAEndpoint, public boost::enable_shared_from_this<CompassEndpoint>{
public:
    static boost::shared_ptr<CompassEndpoint> factory(boost::shared_ptr<Endpoint> connectedTo);
protected:
    CompassEndpoint(boost::shared_ptr<Endpoint> connectedTo);
    virtual void initialize();
public:
    ~CompassEndpoint();
    std::string getId(){return "DigitalCompass";}
protected:
    virtual void deliver_impl(NMEAmsg_ptr msg);
    virtual void deliverAnswer_impl(Answer_ptr answer){};
    virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
    
public:
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
public:
    
private:
    void run_child();
    void update(float yaw, float pitch, float roll);
    void send_messages();
    void list_cmd(Command_ptr command);
    void exit_cmd(Command_ptr command);
    
private:
    enum { max_length = 1024 };
    char data_[max_length];
    std::string data;
    boost::asio::io_service io_service;
    pipe_end *pend;
    boost::thread workerThread;
    boost::asio::deadline_timer timer_;
private:
    float yaw;
    float pitch;
    float roll;
    
};

typedef boost::shared_ptr<CompassEndpoint> CompassEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__CompassEndpoint__) */
