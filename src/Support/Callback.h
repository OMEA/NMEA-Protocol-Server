//
//  Callback.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 17.03.13.
//
//

#ifndef __NMEA_Protocol_Server__Callback__
#define __NMEA_Protocol_Server__Callback__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include "Command.h"
#include "CommandEndpoint.h"

typedef boost::shared_ptr<Command> Command_ptr;
class CommandEndpoint;
typedef boost::shared_ptr<CommandEndpoint> CommandEndpoint_ptr;

class Callback{
public:
    Callback(std::string name): name(name){}
    virtual ~Callback(){}
    virtual void execute(Command_ptr command, CommandEndpoint_ptr instance)=0;
protected:
    std::string name;
};

class MemberCallback: public Callback{
public:
    MemberCallback(std::string name, bool writeable): Callback(name), writeable(writeable){}
protected:
    bool writeable;
};

class BoolCallback: public MemberCallback{
public:
    BoolCallback(std::string name, bool *boolean, bool writeable): MemberCallback(name, writeable), boolean(boolean){}
    virtual void execute(Command_ptr command, CommandEndpoint_ptr instance);
private:
    bool *boolean;
};

class UIntCallback: public MemberCallback{
public:
    UIntCallback(std::string name, unsigned int *uint, unsigned int min, unsigned int max, bool writeable): MemberCallback(name, writeable), min(min), max(max), uint(uint){}
    virtual void execute(Command_ptr command, CommandEndpoint_ptr instance);
private:
    unsigned int min;
    unsigned int max;
    unsigned int *uint;
};

typedef boost::shared_ptr<Callback> Callback_ptr;

#endif /* defined(__NMEA_Protocol_Server__Callback__) */
