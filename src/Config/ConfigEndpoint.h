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

#include "../NMEA/NMEAEndpoint.h"
#include "../NMEA/NMEAmsg.h"

class ConfigEndpoint : public NMEAEndpoint, public boost::enable_shared_from_this<ConfigEndpoint>{
    
public:
    static boost::shared_ptr<ConfigEndpoint> factory(std::string configname=std::string(""));
public:
    ConfigEndpoint();
    virtual ~ConfigEndpoint();
    void load(std::string filename);
    void save(std::string filename=std::string(""));
    void close();
    void deliver_impl(NMEAmsg_ptr msg);
    void deliverAnswer_impl(std::string answer);
    std::string getId();
    virtual void receiveCommand(Command_ptr command);
protected:
    virtual boost::shared_ptr<NMEAEndpoint> v_shared_from_this(){return this->shared_from_this();}
    
private:
    std::string configname;
    std::list<Command_ptr> commands;
};

typedef boost::shared_ptr<ConfigEndpoint> ConfigEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__ConfigEndpoint__) */
