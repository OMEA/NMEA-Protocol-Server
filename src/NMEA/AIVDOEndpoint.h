//
//  AIVDOEndpoint.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 27.03.13.
//
//

#ifndef __NMEA_Protocol_Server__AIVDOEndpoint__
#define __NMEA_Protocol_Server__AIVDOEndpoint__

#include <iostream>
#include "GPSEndpoint.h"

class AIVDOEndpoint: public GPSEndpoint{
public:
    static boost::shared_ptr<AIVDOEndpoint> factory(boost::shared_ptr<Endpoint> connectedTo, unsigned int mmsi);
public:
    std::string getId();
    
    void setMMSI(unsigned int mmsi){this->mmsi = mmsi;}
    unsigned int getMMSI(){return mmsi;}
    
protected:
    AIVDOEndpoint(boost::shared_ptr<Endpoint> connectedTo);
    virtual void initialize();
    
    virtual void deliver_impl(NMEAmsg_ptr msg);
    virtual void deliverAnswer_impl(Answer_ptr answer){};
private:
    void sendPositionReportClassA();
private:
    unsigned int mmsi;
};

typedef boost::shared_ptr<AIVDOEndpoint> AIVDOEndpoint_ptr;

#endif /* defined(__NMEA_Protocol_Server__AIVDOEndpoint__) */
