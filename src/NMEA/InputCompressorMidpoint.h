//
//  InputCompressorMidpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 19.03.13.
//
//

#ifndef __NMEA_Protocol_Server__InputCompressorMidpoint__
#define __NMEA_Protocol_Server__InputCompressorMidpoint__

#include <iostream>

#include "NMEAMidpoint.h"

class InputCompressorMidpoint: public NMEAMidpoint, public boost::enable_shared_from_this<InputCompressorMidpoint>{
public:
    static boost::shared_ptr<InputCompressorMidpoint> factory(boost::shared_ptr<Endpoint> connectedTo);
protected:
    InputCompressorMidpoint(boost::shared_ptr<Endpoint> connectedTo):NMEAMidpoint(connectedTo){}
protected:
    virtual void receive_impl(NMEAmsg_ptr msg);
    virtual boost::shared_ptr<Endpoint> v_shared_from_this(){return this->shared_from_this();}
private:
    std::map<std::string,std::size_t>hashes;
};

typedef boost::shared_ptr<InputCompressorMidpoint> InputCompressorMidpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__InputCompressorMidpoint__) */
