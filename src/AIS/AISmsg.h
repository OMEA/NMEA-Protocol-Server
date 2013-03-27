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
    enum ShipType {
        SHIP_TYPE_NOT_AVAILABLE=0,
        //SHIP_TYPE_RESERVED=1,
        //SHIP_TYPE_RESERVED=2,
        //SHIP_TYPE_RESERVED=3,
        //SHIP_TYPE_RESERVED=4,
        //SHIP_TYPE_RESERVED=5,
        //SHIP_TYPE_RESERVED=6,
        //SHIP_TYPE_RESERVED=7,
        //SHIP_TYPE_RESERVED=8,
        //SHIP_TYPE_RESERVED=9,
        //SHIP_TYPE_RESERVED=10,
        //SHIP_TYPE_RESERVED=11,
        //SHIP_TYPE_RESERVED=12,
        //SHIP_TYPE_RESERVED=13,
        //SHIP_TYPE_RESERVED=14,
        //SHIP_TYPE_RESERVED=15,
        //SHIP_TYPE_RESERVED=16,
        //SHIP_TYPE_RESERVED=17,
        //SHIP_TYPE_RESERVED=18,
        //SHIP_TYPE_RESERVED=19,
        SHIP_TYPE_WIG=20,
        SHIP_TYPE_WIG_HAZARDOUS_A=21,
        SHIP_TYPE_WIG_HAZARDOUS_B=22,
        SHIP_TYPE_WIG_HAZARDOUS_C=23,
        SHIP_TYPE_WIG_HAZARDOUS_D=24,
        //SHIP_TYPE_WIG_RESERVED=25,
        //SHIP_TYPE_WIG_RESERVED=26,
        //SHIP_TYPE_WIG_RESERVED=27,
        //SHIP_TYPE_WIG_RESERVED=28,
        //SHIP_TYPE_WIG_RESERVED=29,
        SHIP_TYPE_FISHING=30,
        SHIP_TYPE_TOWING=31,
        SHIP_TYPE_TOWING_LARGE=32,
        SHIP_TYPE_DREDGING=33,
        SHIP_TYPE_DIVING_OPS=34,
        SHIP_TYPE_MILITARY_OPS=35,
        SHIP_TYPE_SAILING=36,
        SHIP_TYPE_PLEASURE_CRAFT=37,
        //SHIP_TYPE_RESERVED=38,
        //SHIP_TYPE_RESERVED=39,
        SHIP_TYPE_HSC=40,
        SHIP_TYPE_HSC_HAZARDOUS_A=41,
        SHIP_TYPE_HSC_HAZARDOUS_B=42,
        SHIP_TYPE_HSC_HAZARDOUS_C=43,
        SHIP_TYPE_HSC_HAZARDOUS_D=44,
        //SHIP_TYPE_HSC_RESERVED=45,
        //SHIP_TYPE_HSC_RESERVED=46,
        //SHIP_TYPE_HSC_RESERVED=47,
        //SHIP_TYPE_HSC_RESERVED=48,
        SHIP_TYPE_HSC_NO_INFO=49,
        SHIP_TYPE_PILOT=50,
        SHIP_TYPE_SAR=51,
        SHIP_TYPE_TUG=52,
        SHIP_TYPE_PORT_TENDER=53,
        SHIP_TYPE_ANTI_POLLUTION=54,
        SHIP_TYPE_LAW_ENFORCEMENT=55,
        SHIP_TYPE_SPARE_LOCAL1=56,
        SHIP_TYPE_SPARE_LOCAL2=57,
        SHIP_TYPE_MEDICAL=58,
        SHIP_TYPE_NONCOMBAT=59,
        SHIP_TYPE_PASSENGER=60,
        SHIP_TYPE_PASSENGER_HAZARDOUS_A=61,
        SHIP_TYPE_PASSENGER_HAZARDOUS_B=62,
        SHIP_TYPE_PASSENGER_HAZARDOUS_C=63,
        SHIP_TYPE_PASSENGER_HAZARDOUS_D=64,
        //SHIP_TYPE_PASSENGER_RESERVED=65,
        //SHIP_TYPE_PASSENGER_RESERVED=66,
        //SHIP_TYPE_PASSENGER_RESERVED=67,
        //SHIP_TYPE_PASSENGER_RESERVED=68,
        SHIP_TYPE_PASSENGER_NO_INFO=69,
        SHIP_TYPE_CARGO=70,
        SHIP_TYPE_CARGO_HAZARDOUS_A=71,
        SHIP_TYPE_CARGO_HAZARDOUS_B=72,
        SHIP_TYPE_CARGO_HAZARDOUS_C=73,
        SHIP_TYPE_CARGO_HAZARDOUS_D=74,
        //SHIP_TYPE_CARGO_RESERVED=75,
        //SHIP_TYPE_CARGO_RESERVED=76,
        //SHIP_TYPE_CARGO_RESERVED=77,
        //SHIP_TYPE_CARGO_RESERVED=78,
        SHIP_TYPE_CARGO_NO_INFO=79,
        SHIP_TYPE_TANKER=80,
        SHIP_TYPE_TANKER_HAZARDOUS_A=81,
        SHIP_TYPE_TANKER_HAZARDOUS_B=82,
        SHIP_TYPE_TANKER_HAZARDOUS_C=83,
        SHIP_TYPE_TANKER_HAZARDOUS_D=84,
        //SHIP_TYPE_TANKER_RESERVED=85,
        //SHIP_TYPE_TANKER_RESERVED=86,
        //SHIP_TYPE_TANKER_RESERVED=87,
        //SHIP_TYPE_TANKER_RESERVED=88,
        SHIP_TYPE_TANKER_NO_INFO=89,
        SHIP_TYPE_OTHER=90,
        SHIP_TYPE_OTHER_HAZARDOUS_A=91,
        SHIP_TYPE_OTHER_HAZARDOUS_B=92,
        SHIP_TYPE_OTHER_HAZARDOUS_C=93,
        SHIP_TYPE_OTHER_HAZARDOUS_D=94,
        //SHIP_TYPE_OTHER_RESERVED=95,
        //SHIP_TYPE_OTHER_RESERVED=96,
        //SHIP_TYPE_OTHER_RESERVED=97,
        //SHIP_TYPE_OTHER_RESERVED=98,
        SHIP_TYPE_OTHER_NO_INFO=99
    };
    enum PositionFixType{
        POSITION_FIX_UNDEFINED=0,
        POSITION_FIX_GPS=1,
        POSITION_FIX_GLONASS=2,
        POSITION_FIX_GPS_GLONASS=3,
        POSITION_FIX_LORAN_C=4,
        POSITION_FIX_CHAYKA=5,
        POSITION_FIX_INTEGRATED=6,
        POSITION_FIX_SURVEYED=7,
        POSITION_FIX_GALILEO=8,
        POSITION_FIX_RESERVED=9,
        //POSITION_FIX_RESERVED=10,
        //POSITION_FIX_RESERVED=11,
        //POSITION_FIX_RESERVED=12,
        //POSITION_FIX_RESERVED=13,
        //POSITION_FIX_RESERVED=14,
        //POSITION_FIX_RESERVED=15,
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
