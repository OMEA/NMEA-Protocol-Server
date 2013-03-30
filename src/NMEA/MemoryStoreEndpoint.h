//
//  MemoryStoreEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 30.03.13.
//
//

#ifndef __NMEA_Protocol_Server__MemoryStoreEndpoint__
#define __NMEA_Protocol_Server__MemoryStoreEndpoint__

#include <iostream>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../NMEA/NMEAEndpoint.h"
#include "../NMEA/NMEAmsg.h"

class MemoryStoreEndpoint : public NMEAEndpoint, public boost::enable_shared_from_this<MemoryStoreEndpoint>{
public:
    static boost::shared_ptr<MemoryStoreEndpoint> factory(boost::shared_ptr<Endpoint> connectedTo, std::string name, unsigned int store_size=20);
public:
    using NMEAEndpoint::receive;
protected:
    MemoryStoreEndpoint(boost::shared_ptr<Endpoint> connectedTo, std::string name, unsigned int store_size);
    virtual void initialize();
public:
    virtual ~MemoryStoreEndpoint(){}
    void deliver_impl(NMEAmsg_ptr msg);
    void deliverAnswer_impl(Answer_ptr answer);
    std::string getId();
protected:
    virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
private:
    void relay_cmd(Command_ptr command);
    
private:
    std::deque<Message_ptr> message_queue;
    unsigned int store_size;
    std::string name;
};

typedef boost::shared_ptr<MemoryStoreEndpoint> MemoryStoreEndpoint_ptr;


#endif /* defined(__NMEA_Protocol_Server__MemoryStoreEndpoint__) */
