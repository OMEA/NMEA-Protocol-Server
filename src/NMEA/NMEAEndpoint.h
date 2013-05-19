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
private:
    void update_stats_deliver(NMEAmsg_ptr msg);
    void update_stats_receive(NMEAmsg_ptr msg);
protected:
    bool input;
    bool output;
    bool portmirror;
    bool checksum;
    bool check_checksum;
    long long int incompress_messages;
    long long int outcompress_messages;
protected:
    void add_midpoint_cmd(Command_ptr command);
    void print_stats_cmd(Command_ptr command);
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
    
    bool stats_enabled;
    std::list<std::pair<boost::posix_time::ptime, size_t> > in_stat_list;
    unsigned int in_total_size;
    unsigned long in_msg_from_start;
    unsigned long in_byte_from_start;
    std::list<std::pair<boost::posix_time::ptime, size_t> > out_stat_list;
    unsigned int out_total_size;
    unsigned long out_msg_from_start;
    unsigned long out_byte_from_start;
    unsigned int stat_list_size;
};

typedef boost::shared_ptr<NMEAEndpoint> NMEAEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__NMEAEndpoint__) */
