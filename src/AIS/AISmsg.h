//
//  AISmsg.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#ifndef __NMEA_Protocol_Server__AISmsg__
#define __NMEA_Protocol_Server__AISmsg__

#include <iostream>
#include <boost/shared_ptr.hpp>

uint64_t ubits(const char buf[], unsigned int start, unsigned int width, bool le);
int64_t sbits(const char buf[], unsigned int start, unsigned int width, bool le);

class AISmsg{
public:
    static boost::shared_ptr<AISmsg> factory(std::string parseMsg);
public:
    enum MsgType {
        POSITION_REPORT_CLASS_A=1,
        POSITION_REPORT_CLASS_A_2=2,
        POSITION_REPORT_CLASS_A_3=3,
        BASE_STATION_REPORT=4
    };
public:
    static std::string deArmoring(const std::string deArmoring);
    static std::string armoring(const std::string armoringStr);
public:
    virtual ~AISmsg(){}
    virtual void setType(MsgType type){this->type = type;}
    virtual MsgType getType(){return type;}
    void setRepeat(unsigned char repeat){this->repeat = repeat;}
    unsigned char getRepeat(){return repeat;}
    void setMMSI(unsigned int mmsi){this->mmsi = mmsi;}
    unsigned int getMMSI(){return mmsi;}
protected:
    AISmsg(std::string parseMsg);
private:
    MsgType type;
    unsigned char repeat;
    unsigned int mmsi;
};

typedef boost::shared_ptr<AISmsg> AISmsg_ptr;
#endif /* defined(__NMEA_Protocol_Server__AISmsg__) */
