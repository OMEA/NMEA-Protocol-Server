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
#include <map>
#include <boost/assign.hpp>
#include <boost/shared_ptr.hpp>


unsigned long getUBits(const std::string from, unsigned int offset, unsigned int length);
void setUBits(std::string &to, unsigned int offset, unsigned int length, unsigned long value);
long getSBits(const std::string from, unsigned int offset, unsigned int length);
void setSBits(std::string &to, unsigned int offset, unsigned int length, long value);
std::string getStringBits(const std::string from, unsigned int offset, unsigned int length);
void setStringBits(std::string &to, unsigned int offset, unsigned int length, std::string str);

class AISmsg{
public:
    static boost::shared_ptr<AISmsg> factory(std::string parseMsg);
public:
    enum MsgType {
        UNDEFINED=0,
        POSITION_REPORT_CLASS_A=1,
        POSITION_REPORT_CLASS_A_2=2,
        POSITION_REPORT_CLASS_A_3=3,
        BASE_STATION_REPORT=4,
        STATIC_VOYAGE_RELATED = 5
    };
    enum NavigationStatus {
        UNDER_WAY_USING_ENGINE=0,
        AT_ANCHOR=1,
        NOT_UNDER_COMMAND=2,
        RESTRICTED_MANOEUVERABILITY=3,
        CONSTRAINED_BY_HER_DRAUGHT=4,
        MOORED=5,
        AGROUND=6,
        ENGAGED_IN_FISHING=7,
        UNDER_WAY_SAILING=8,
        //RESERVED=9,
        //RESERVED=10,
        //RESERVED=11,
        //RESERVED=12,
        //RESERVED=13,
        //RESERVED=14,
        NOT_DEFINED=15
    };
    enum ManeuverIndicator {
        NOT_AVAILABLE=0,
        NO_SPECIAL_MANEUVER=1,
        NO_SPECIAL=2
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
    void setBitLength(unsigned int bitLength){this->bitLength = bitLength;}
    unsigned int getBitLength(){return bitLength;}
    
    std::string toCodedStr();
protected:
    AISmsg(std::string parseMsg);
    AISmsg();
    virtual void code(std::string &code);
private:
    unsigned int bitLength;
    MsgType type;
    unsigned char repeat;
    unsigned int mmsi;
public:
    static const std::map<char,unsigned char> char2SixBit;
    static const char sixBit2Char[64];
};

typedef boost::shared_ptr<AISmsg> AISmsg_ptr;
#endif /* defined(__NMEA_Protocol_Server__AISmsg__) */
