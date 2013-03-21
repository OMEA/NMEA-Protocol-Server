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
    virtual ~CommandEndpoint(){}
    virtual void receive(Command_ptr command);
    virtual void deliver(Command_ptr command);
    virtual void deliver(Answer_ptr answer);
private:
    void id_cmd(Command_ptr command);
    
protected:
    void unregisterCmd(std::string name);
    void registerBoolCmd(std::string name, std::string speakingName, std::string description, bool *boolean, bool defaultValue, bool writeable=true);
    void registerUIntCmd(std::string name, std::string speakingName, std::string description, unsigned int *uint, unsigned int defaultValue, unsigned int min=0, unsigned int max=UINT_MAX, bool writeable=true);
    void registerIntCmd(std::string name, std::string speakingName, std::string description, int *int_, int defaultValue, int min, int max, bool writeable);
    void registerStringVectorCmd(std::string name, std::string speakingName, std::string description, std::vector<std::string> *vector, bool writeable);
    void registerVoidCmd(std::string name, std::string speakingName, std::string description, boost::function<void (Command_ptr)> &func);
    virtual void registerEndpoint();
    virtual void deliverAnswer_impl(Answer_ptr answer)=0;
    virtual boost::shared_ptr<Endpoint> v_shared_from_this()=0;
private:
    std::map<std::string,Callback_ptr> registred_commands;
    
protected:
    CommandEndpoint(){}
    CommandEndpoint(boost::shared_ptr<Endpoint> connectedTo);
    virtual void initialize();
};

typedef boost::shared_ptr<CommandEndpoint> CommandEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__CommandEndpoint__) */
