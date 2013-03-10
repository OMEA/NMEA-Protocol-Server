//
//  FileEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 26.02.13.
//  Copyright (c) 2013 Till Steinbach. All rights reserved.
//

#ifndef __NMEA_Protocol_Server__FileEndpoint__
#define __NMEA_Protocol_Server__FileEndpoint__

#include <iostream>
#include <fstream>
#include <queue>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../NMEA/NMEAEndpoint.h"
#include "../NMEA/NMEAMsg.h"

class FileEndpoint : public NMEAEndpoint, public boost::enable_shared_from_this<FileEndpoint>{
    
public:
    FileEndpoint();
    ~FileEndpoint();
    void open(char *filename);
    void close();
    void deliver_impl(NMEAmsg_ptr msg);
    void deliverAnswer_impl(std::string answer);
    std::string getId();
    virtual void receiveCommand(Command_ptr command);
protected:
    virtual boost::shared_ptr<NMEAEndpoint> v_shared_from_this(){return this->shared_from_this();}
    
private:
    std::deque<NMEAmsg_ptr> message_queue;
    std::ofstream file_stream;
    std::string filename;
};

typedef boost::shared_ptr<FileEndpoint> FileEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__FileEndpoint__) */
