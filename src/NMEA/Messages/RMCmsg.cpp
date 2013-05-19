//
//  RMCmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#include "RMCmsg.h"
#include <boost/regex.hpp>

#define MATCH_HOUR 1
#define MATCH_MINUTE MATCH_HOUR+1
#define MATCH_SECOND MATCH_MINUTE+1
#define MATCH_SUBSECOND MATCH_SECOND+1
#define MATCH_STATE MATCH_SUBSECOND+1
#define MATCH_LAT_DEG MATCH_STATE+1
#define MATCH_LAT_MIN MATCH_LAT_DEG+1
#define MATCH_LAT_SIGN MATCH_LAT_MIN+1
#define MATCH_LON_DEG MATCH_LAT_SIGN+1
#define MATCH_LON_MIN MATCH_LON_DEG+1
#define MATCH_LON_SIGN MATCH_LON_MIN+1
#define MATCH_SPEED MATCH_LON_SIGN+1
#define MATCH_COURSE MATCH_SPEED+1
#define MATCH_DAY MATCH_COURSE+1
#define MATCH_MONTH MATCH_DAY+1
#define MATCH_YEAR MATCH_MONTH+1

RMCmsg::RMCmsg(std::string parseMsg, Endpoint_ptr sender, bool check_checksum): NMEAmsg(parseMsg, sender, check_checksum){
    boost::regex reg("^(\\d{2})(\\d{2})(\\d{2})(\\.\\d{1,3})?,([AV]),(\\d{2})(\\d{2}\\.\\d{4}),([NS]),(\\d{2-3})(\\d{2}\\.\\d{4}),([EW]),(\\d{1,2}\\.\\d{1,3}),(\\d{1,3}\\.\\d{1,3}),(\\d{2})(\\d{2})(\\d{2}),(\\d{1,3}\\.\\d{1,3})?,([EW])?,([ADENS])$");
    boost::cmatch matches;
    if(boost::regex_search(this->getMsg().c_str(), matches, reg)){
        std::string ts("20"+matches[MATCH_YEAR]+"-"+matches[MATCH_MONTH]+"-"+matches[MATCH_DAY]+" "+matches[MATCH_HOUR]+":"+matches[MATCH_MINUTE]+":"+matches[MATCH_SECOND]+matches[MATCH_SUBSECOND]);
        try{
            time = boost::posix_time::time_from_string(ts);
        }
        catch(const boost::bad_lexical_cast&){
            std::cerr << "Problem with command format. Cannot parse date:" << ts << std::endl;
            return;
        }
        if(matches[MATCH_STATE]=="A"){
            state = ACTIVE;
        }
        else{
            state = VOID;
        }
        if(matches[MATCH_LAT_SIGN]=="N"){
            latitude.setSign(Latitude::NORTH);
        }
        else{
            latitude.setSign(Latitude::SOUTH);
        }
        if(matches[MATCH_LON_SIGN]=="E"){
            longitude.setSign(Longitude::EAST);
        }
        else{
            longitude.setSign(Longitude::WEST);
        }
        try{
            latitude.setDegrees(boost::lexical_cast<unsigned short>(matches[MATCH_LAT_DEG]));
            latitude.setMinutes(boost::lexical_cast<double>(matches[MATCH_LAT_MIN]));
            longitude.setDegrees(boost::lexical_cast<unsigned short>(matches[MATCH_LON_DEG]));
            longitude.setMinutes(boost::lexical_cast<double>(matches[MATCH_LON_MIN]));
        }
        catch(const boost::bad_lexical_cast&){
            std::cerr << "Problem with command format. Cannot parse latitude or longitude" << std::endl;
            return;
        }
        try{
            speed = boost::lexical_cast<double>(matches[MATCH_SPEED]);
            course = boost::lexical_cast<double>(matches[MATCH_COURSE]);
        }
        catch(const boost::bad_lexical_cast&){
            std::cerr << "Problem with command format. Cannot parse speed or course" << std::endl;
            return;
        }
    }
    else{
        throw std::invalid_argument("Cannot parse RMC message!");
    }
}