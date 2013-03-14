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

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "../NMEA/NMEAEndpoint.h"
#include "../NMEA/NMEAmsg.h"

class FileEndpoint : public NMEAEndpoint, public boost::enable_shared_from_this<FileEndpoint>{
public:
    static boost::shared_ptr<FileEndpoint> factory(std::string filename=std::string(""));
public:
    FileEndpoint();
    ~FileEndpoint();
    void open(std::string filename);
    void close();
    void deliver_impl(NMEAmsg_ptr msg);
    void deliverAnswer_impl(std::string answer);
    std::string getId();
    virtual void receiveCommand(Command_ptr command);
    void startPlayback(boost::posix_time::ptime from,  boost::posix_time::ptime to);
    void stopPlayback(void);
    void record(void);
protected:
    virtual boost::shared_ptr<NMEAEndpoint> v_shared_from_this(){return this->shared_from_this();}
    
private:
    void play(boost::posix_time::ptime from,  boost::posix_time::ptime to);
    std::deque<NMEAmsg_ptr> message_queue;
    std::fstream file_stream;
    std::string filename;
    bool playback=false;
    bool stopPlaybackNow=true;
    boost::thread playbackThread;
};

typedef boost::shared_ptr<FileEndpoint> FileEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__FileEndpoint__) */
