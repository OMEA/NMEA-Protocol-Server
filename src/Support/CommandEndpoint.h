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

#include "Endpoint.h"
#include "Command.h"
#include "Answer.h"

class Command;
typedef boost::shared_ptr<Command> Command_ptr;

class CommandEndpoint: public Endpoint
{
public:
    virtual ~CommandEndpoint(){}
    virtual void receive(Command_ptr command);
    virtual void deliver(Command_ptr command);
    virtual void deliver(Answer_ptr answer);
protected:
    virtual void deliverAnswer_impl(Answer_ptr answer)=0;
    virtual boost::shared_ptr<Endpoint> v_shared_from_this()=0;
};

typedef boost::shared_ptr<CommandEndpoint> CommandEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__CommandEndpoint__) */
