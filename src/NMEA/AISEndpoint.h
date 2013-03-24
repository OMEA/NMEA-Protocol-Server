//
//  AISEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#ifndef __NMEA_Protocol_Server__AISEndpoint__
#define __NMEA_Protocol_Server__AISEndpoint__

#include <iostream>
#include "GPSEndpoint.h"

class AISEndpoint: public GPSEndpoint{
public:
    static boost::shared_ptr<AISEndpoint> factory(boost::shared_ptr<Endpoint> connectedTo);
protected:
    AISEndpoint(boost::shared_ptr<Endpoint> connectedTo);
private:
    std::string ais_decode(std::string bitstream);
};

typedef boost::shared_ptr<AISEndpoint> AISEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__AISEndpoint__) */
