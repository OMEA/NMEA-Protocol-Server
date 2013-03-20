//
//  NMEAEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 23.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#ifndef __NMEA_Protocol_Server__NMEAEndpoint__
#define __NMEA_Protocol_Server__NMEAEndpoint__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "NMEAmsg.h"
#include "../Support/Command.h"
#include "../Support/Endpoint.h"

class NMEAEndpoint: public CommandEndpoint
{
public:
    using CommandEndpoint::deliver;
    using CommandEndpoint::receive;
    
protected:
    NMEAEndpoint();
    NMEAEndpoint(boost::shared_ptr<Endpoint> connectedTo);
    virtual void initialize();
public:
    virtual ~NMEAEndpoint(){}
    virtual void deliver(NMEAmsg_ptr msg);
    virtual void receive(NMEAmsg_ptr msg);
    
    virtual void receive(Command_ptr command);
protected:
    virtual void deliver_impl(NMEAmsg_ptr msg)=0;
    virtual void receive_impl(NMEAmsg_ptr msg);
    virtual boost::shared_ptr<Endpoint> v_shared_from_this()=0;
    
protected:
    bool input;
    bool output;
    bool portmirror;
    bool checksum;
    int incompress_messages;
    int outcompress_messages;
private:
    void add_midpoint_cmd(Command_ptr command);
private:
    bool compress(NMEAmsg_ptr msg, std::map<std::string,std::pair<std::size_t, unsigned int> > *hashes, int num_messages);
    bool black_and_white(NMEAmsg_ptr msg, const std::vector<std::string> *blacklist, const std::vector<std::string> *whitelist);
private:
    std::map<std::string,std::pair<std::size_t, unsigned int> >incompressor_hashes;
    std::map<std::string,std::pair<std::size_t, unsigned int> >outcompressor_hashes;
    std::vector<std::string> in_black;
    std::vector<std::string> in_white;
    std::vector<std::string> out_black;
    std::vector<std::string> out_white;
};

typedef boost::shared_ptr<NMEAEndpoint> NMEAEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__NMEAEndpoint__) */
