//
//  FileEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 26.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#ifndef __NMEA_Protocol_Server__ConfigEndpoint__
#define __NMEA_Protocol_Server__ConfigEndpoint__

#include <iostream>
#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../Support/CommandEndpoint.h"
#include "../Support/Command.h"

class ConfigEndpoint : public CommandEndpoint, public boost::enable_shared_from_this<ConfigEndpoint>{
    
public:
    static boost::shared_ptr<ConfigEndpoint> factory(boost::shared_ptr<Endpoint> connectedTo, std::string configname=std::string(""));
protected:
    ConfigEndpoint();
    ConfigEndpoint(boost::shared_ptr<Endpoint> connectedTo);
public:
    virtual ~ConfigEndpoint();
    void load(std::string filename);
    void save(std::string filename=std::string(""));
    void close();
    std::string getId();
    virtual void receive(Command_ptr command);
protected:
    virtual void deliverAnswer_impl(Answer_ptr answer);
    virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
    
private:
    std::string configname;
    std::list<Command_ptr> commands;
};

typedef boost::shared_ptr<ConfigEndpoint> ConfigEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__ConfigEndpoint__) */
