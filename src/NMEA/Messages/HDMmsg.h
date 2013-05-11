//
//  HDMmsg.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 10.05.13.
//
//

#ifndef __NMEA_Protocol_Server__HDMmsg__
#define __NMEA_Protocol_Server__HDMmsg__

#include <iostream>
#include "../NMEAmsg.h"
#include "navobjects.h"

class HDMmsg: public NMEAmsg {
public:
    HDMmsg(std::string parseMsg, Endpoint_ptr sender, bool check_checksum=false);
    HDMmsg(Endpoint_ptr sender);
    
        
    void setHeadingMagnetic(float headingMagnetic){
        if(headingMagnetic>=0){this->headingMagnetic=headingMagnetic;}
        else{this->headingMagnetic=360-headingMagnetic;}
    }
    float getHeadingMagnetic(){return headingMagnetic;}
    
    virtual void setMsg(std::string msg);
    virtual const std::string getMsg()const;
private:
    float headingMagnetic;
};

typedef boost::shared_ptr<HDMmsg> HDMmsg_ptr;

#endif /* defined(__NMEA_Protocol_Server__HDMmsg__) */
