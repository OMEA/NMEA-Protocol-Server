//
//  CommandEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 14.03.13.
//
//

#ifndef __NMEA_Protocol_Server__CommandEndpoint__
#define __NMEA_Protocol_Server__CommandEndpoint__

#include <iostream>
#include <climits>

#include "Endpoint.h"
#include "Callback.h"
#include "Command.h"
#include "Answer.h"

class Command;
typedef boost::shared_ptr<Command> Command_ptr;
class Callback;
typedef boost::shared_ptr<Callback> Callback_ptr;

class CommandEndpoint: public Endpoint
{
public:
    CommandEndpoint(){}
    CommandEndpoint(boost::shared_ptr<Endpoint> connectedTo);
    virtual ~CommandEndpoint(){}
    virtual void receive(Command_ptr command);
    virtual void deliver(Command_ptr command);
    virtual void deliver(Answer_ptr answer);
protected:
    void registerBoolCmd(std::string name, bool *boolean, bool defaultValue, bool writeable=true);
    void registerUIntCmd(std::string name, unsigned int *uint, unsigned int defaultValue, unsigned int min=0, unsigned int max=UINT_MAX, bool writeable=true);
    virtual void registerEndpoint();
    virtual void deliverAnswer_impl(Answer_ptr answer)=0;
    virtual boost::shared_ptr<Endpoint> v_shared_from_this()=0;
private:
    std::map<std::string,Callback_ptr> registred_commands;
};

typedef boost::shared_ptr<CommandEndpoint> CommandEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__CommandEndpoint__) */
