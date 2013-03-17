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
    using NMEAEndpoint::receive;
public:
    AsyncEndpoint();
    AsyncEndpoint(boost::shared_ptr<Endpoint> connectedTo);
    ~AsyncEndpoint();
    
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);
    virtual void start();
    virtual void stop();
    
    virtual void receive(Command_ptr command);
    
    void setSessionId(std::string sessionId){this->sessionId = sessionId;}
    std::string getSessionId(){return sessionId;}
protected:
    void setAOStream(T* aostream){this->aostream=aostream;}
    T* getAOStream(){return aostream;}
    void deliver_impl(NMEAmsg_ptr msg);
    void deliverAnswer_impl(Answer_ptr answer);
    virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
    
private:
    T* aostream;
    std::deque<Message_ptr> message_queue;
    boost::condition_variable message_queueCond;
    boost::mutex message_queueMutex;
    unsigned int message_queue_size;
    enum { max_length = 1024 };
    char data_[max_length];
    char data_send_[max_length];
    std::string data;
    bool persist;
    bool isActive;
    std::string sessionId;
private:
    static std::map<std::string, boost::shared_ptr<AsyncEndpoint<T> > > sessions;
public:
    static void deactivateSession(boost::shared_ptr<AsyncEndpoint<T> > session);
    static boost::shared_ptr<AsyncEndpoint<T> > activateSession(boost::shared_ptr<AsyncEndpoint<T> > session);
};

#endif /* defined(__NMEA_Protocol_Server__AsyncEndpoint__) */
