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
#include <boost/function.hpp>
#include "Command.h"
#include "CommandEndpoint.h"

typedef boost::shared_ptr<Command> Command_ptr;
class CommandEndpoint;
typedef boost::shared_ptr<CommandEndpoint> CommandEndpoint_ptr;

class Callback{
public:
    Callback(std::string name, std::string speakingName, std::string description): name(name), speakingName(speakingName), description(description){}
    virtual ~Callback(){}
    virtual void execute(Command_ptr command, CommandEndpoint_ptr instance)=0;
    
    virtual const std::string to_str() const;
    friend std::ostream& operator<< (std::ostream &out, Callback &callback);
protected:
    std::string name;
    std::string speakingName;
    std::string description;
};

class MemberCallback: public Callback{
public:
    MemberCallback(std::string name, std::string speakingName, std::string description, bool writeable): Callback(name, speakingName, description), writeable(writeable){}
protected:
    bool writeable;
};

class BoolCallback: public MemberCallback{
public:
    BoolCallback(std::string name, std::string speakingName, std::string description, bool *boolean, bool writeable): MemberCallback(name, speakingName, description, writeable), boolean(boolean){}
    virtual void execute(Command_ptr command, CommandEndpoint_ptr instance);
    virtual const std::string to_str() const;
private:
    bool *boolean;
};

class UIntCallback: public MemberCallback{
public:
    UIntCallback(std::string name, std::string speakingName, std::string description, unsigned int *uint, unsigned int min, unsigned int max, bool writeable): MemberCallback(name, speakingName, description, writeable), min(min), max(max), uint(uint){}
    virtual void execute(Command_ptr command, CommandEndpoint_ptr instance);
    virtual const std::string to_str() const;
private:
    unsigned int min;
    unsigned int max;
    unsigned int *uint;
};

class IntCallback: public MemberCallback{
public:
    IntCallback(std::string name, std::string speakingName, std::string description, int *int_, int min, int max, bool writeable): MemberCallback(name, speakingName, description, writeable), min(min), max(max), int_(int_){}
    virtual void execute(Command_ptr command, CommandEndpoint_ptr instance);
    virtual const std::string to_str() const;
private:
    int min;
    int max;
    int *int_;
};

class StringCallback: public MemberCallback{
public:
    StringCallback(std::string name, std::string speakingName, std::string description, std::string *string, bool writeable): MemberCallback(name, speakingName, description, writeable), string(string){}
    virtual void execute(Command_ptr command, CommandEndpoint_ptr instance);
    virtual const std::string to_str() const;
private:
    std::string *string;
};

class StringVectorCallback: public MemberCallback{
public:
    StringVectorCallback(std::string name, std::string speakingName, std::string description, std::vector<std::string> *vector, bool writeable): MemberCallback(name, speakingName, description, writeable), vector(vector){}
    virtual void execute(Command_ptr command, CommandEndpoint_ptr instance);
    virtual const std::string to_str() const;
private:
    std::vector<std::string> *vector;
};

class VoidCallback: public Callback{
public:
    VoidCallback(std::string name, std::string speakingName, std::string description, boost::function<void (Command_ptr)> func): Callback(name, speakingName, description), func(func){}
    virtual void execute(Command_ptr command, CommandEndpoint_ptr instance);
private:
    boost::function<void (Command_ptr)> func;
};

typedef boost::shared_ptr<Callback> Callback_ptr;

#endif /* defined(__NMEA_Protocol_Server__Callback__) */
