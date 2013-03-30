//
//  MemoryStoreEndpoint.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 30.03.13.
//
//

#include "MemoryStoreEndpoint.h"

boost::shared_ptr<MemoryStoreEndpoint> MemoryStoreEndpoint::factory(boost::shared_ptr<Endpoint> connectedTo, std::string name, unsigned int store_size) {
    MemoryStoreEndpoint_ptr memoryStoreEndpoint(new MemoryStoreEndpoint(connectedTo, name, store_size));
    memoryStoreEndpoint->initialize();
    return memoryStoreEndpoint;
}

MemoryStoreEndpoint::MemoryStoreEndpoint(boost::shared_ptr<Endpoint> connectedTo, std::string name, unsigned int store_size): NMEAEndpoint(connectedTo){
    this->store_size = store_size;
    this->name = name;
}

void MemoryStoreEndpoint::initialize(){
    NMEAEndpoint::initialize();
    registerUIntCmd("size","Maximum number of stored messages", "Defines the maximum number of consecutive messages that are stored.", &this->store_size, this->store_size, 1, std::numeric_limits<unsigned int>::max(), true);
    registerStringCmd("name","Name", "Define Name of Endpoint",  &this->name, this->name, true);
    boost::function<void (Command_ptr)> func = boost::bind(&MemoryStoreEndpoint::relay_cmd, this, _1);
    registerVoidCmd("relay","Relay stored messages", "When called, stored messages are relayed to the sender of this command.",  func);
    registerEndpoint();
}

void MemoryStoreEndpoint::relay_cmd(Command_ptr command){
    for(std::deque<Message_ptr>::iterator message=message_queue.begin();message!=message_queue.end();++message){
        NMEAmsg_ptr nmeamsg = boost::dynamic_pointer_cast<NMEAmsg>(*message);
        if(nmeamsg){
            NMEAEndpoint_ptr nmeaEndpoint = boost::dynamic_pointer_cast<NMEAEndpoint>(command->getSender());
            if(nmeaEndpoint){
                nmeaEndpoint->deliver(nmeamsg);
            }
        }
        Answer_ptr answer = boost::dynamic_pointer_cast<Answer>(*message);
        if(answer){
            CommandEndpoint_ptr commandEndpoint = boost::dynamic_pointer_cast<CommandEndpoint>(command->getSender());
            commandEndpoint->deliver(answer);
        }
    }
}

std::string MemoryStoreEndpoint::getId(){
    return "Memory:"+name;
}



void MemoryStoreEndpoint::deliver_impl(NMEAmsg_ptr msg){
    if(message_queue.size()>=store_size){
        message_queue.pop_front();
    }
    message_queue.push_back(msg);
}
void MemoryStoreEndpoint::deliverAnswer_impl(Answer_ptr answer){
    if(message_queue.size()>=store_size){
        message_queue.pop_front();
    }
    message_queue.push_back(answer);
}