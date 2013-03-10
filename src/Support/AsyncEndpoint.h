//
//  AsyncEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 27.02.13.
//
//

#ifndef __NMEA_Protocol_Server__AsyncEndpoint__
#define __NMEA_Protocol_Server__AsyncEndpoint__

#include <iostream>
#include <queue>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <boost/thread/condition_variable.hpp>
#include <boost/asio/buffer.hpp>

#include "../NMEA/NMEAmsg.h"
#include "../NMEA/NMEAEndpoint.h"

template <class T>
class AsyncEndpoint: public NMEAEndpoint, public boost::enable_shared_from_this< AsyncEndpoint<T> >{

public:
    ~AsyncEndpoint();
    void setAOStream(T* aostream){this->aostream=aostream;}
    void deliver_impl(NMEAmsg_ptr msg);
    void deliverAnswer_impl(std::string answer);
    
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);
    void start();
    void stop();
    
    virtual void receiveCommand(Command_ptr command);
    
protected:
    virtual boost::shared_ptr<NMEAEndpoint> v_shared_from_this(){return this->shared_from_this();}
    
private:
    T* aostream;
    std::deque<NMEAmsg_ptr> message_queue;
    boost::condition_variable message_queueCond;
    boost::mutex message_queueMutex;
    std::deque<std::string> answer_queue;
    boost::condition_variable answer_queueCond;
    boost::mutex answer_queueMutex;
    enum { max_length = 1024 };
    char data_[max_length];
    char data_send_[max_length];
    std::string data;
};


#endif /* defined(__NMEA_Protocol_Server__AsyncEndpoint__) */