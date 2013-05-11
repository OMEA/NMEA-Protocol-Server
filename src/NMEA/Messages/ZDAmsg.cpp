//
//  ZDAmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 10.05.13.
//
//

#include "ZDAmsg.h"
#include <boost/regex.hpp>

#define MATCH_HOUR 1
#define MATCH_MINUTE MATCH_HOUR+1
#define MATCH_SECOND MATCH_MINUTE+1
#define MATCH_SUBSECOND MATCH_SECOND+1
#define MATCH_DAY MATCH_SUBSECOND+1
#define MATCH_MONTH MATCH_DAY+1
#define MATCH_YEAR MATCH_MONTH+1
#define MATCH_ZONE_HOURS MATCH_YEAR+1
#define MATCH_ZONE_MINUTES MATCH_ZONE_HOURS+1

ZDAmsg::ZDAmsg(std::string parseMsg, Endpoint_ptr sender, bool check_checksum): NMEAmsg(parseMsg, sender, check_checksum), zone(boost::local_time::posix_time_zone("UTC+0")){
    setMsg(NMEAmsg::getMsg());
}


void ZDAmsg::setMsg(std::string msg){
    boost::regex reg("^(\\d{2})(\\d{2})(\\d{2})(\\.\\d{1,3})?,(\\d{2}),(\\d{2}),(\\d{4}),(\\d{2}),(\\d{2})$");
    boost::cmatch matches;
    if(boost::regex_search(msg.c_str(), matches, reg)){
        std::string ts(matches[MATCH_YEAR]+"-"+matches[MATCH_MONTH]+"-"+matches[MATCH_DAY]+" "+matches[MATCH_HOUR]+":"+matches[MATCH_MINUTE]+":"+matches[MATCH_SECOND]+matches[MATCH_SUBSECOND]);
        try{
            std::string sign = (boost::lexical_cast<int>(matches[MATCH_ZONE_HOURS])>=0?"+":"-");
            std::string tz("UTC"+sign+matches[MATCH_ZONE_HOURS]+":"+matches[MATCH_ZONE_MINUTES]);
            zone = boost::local_time::posix_time_zone(tz);
            time_date = boost::posix_time::time_from_string(ts);
        }
        catch(const boost::bad_lexical_cast&){
            std::cerr << "Problem with command format. Cannot parse date:" << ts << std::endl;
            return;
        }
    }
    else{
        throw std::invalid_argument("Cannot parse RMC message!");
    }
}
const std::string ZDAmsg::getMsg()const{
    std::ostringstream oss;
    //boost::posix_time::time_duration utc_offset = zone.base_utc_offset();
    //time_facet *facet = new time_facet("%d-%b-%Y %H:%M:%S");
    oss << "hhmmss.ss,dd,mm,yyyy,xx,yy";
    return oss.str();
}

ZDAmsg::ZDAmsg(Endpoint_ptr sender): NMEAmsg(sender,'$',"ZDA"), zone(boost::local_time::posix_time_zone("UTC+0")){
    
}