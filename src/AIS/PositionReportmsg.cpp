//
//  PositionReportmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#include "PositionReportmsg.h"

boost::shared_ptr<PositionReportmsg> PositionReportmsg::factory(std::string parseMsg) {
    boost::shared_ptr<PositionReportmsg> ret(new PositionReportmsg(parseMsg));
    return ret;
}

PositionReportmsg::PositionReportmsg(std::string parseMsg): AISmsg(parseMsg){
        lon = sbits(parseMsg.c_str(), 61, 28, false);
        lat = sbits(parseMsg.c_str(), 89, 27, false);
    }