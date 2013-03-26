//
//  navobjects.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#include "navobjects.h"
#include <sstream>

const std::string LatLong::to_str() const{
    std::ostringstream oss;
    oss << getDegrees() << "°" << getMinutes() << "\"";
    return oss.str();
}

std::ostream& operator<<(std::ostream &strm, const LatLong &latlong) {
    return strm << latlong.to_str();
}

Latitude::Latitude(double minutes){
    if(minutes>=0){
        setSign(Latitude::NORTH);
    }
    else{
        setSign(Latitude::SOUTH);
    }
    setMinutes(fabs(minutes));
    setDegrees(fabs(minutes/60));
}

const std::string Latitude::to_str() const{
    std::ostringstream oss;
    oss << LatLong::to_str() << ' ' << ((getSign()==Latitude::NORTH)?"N":"S");
    return oss.str();
}

std::ostream& operator<<(std::ostream &strm, const Latitude &latitude) {
    return strm << latitude.to_str();
}

Longitude::Longitude(double minutes){
    if(minutes>=0){
        setSign(Longitude::EAST);
    }
    else{
        setSign(Longitude::WEST);
    }
    setMinutes(fabs(minutes));
    setDegrees(fabs(minutes/60));
}

const std::string Longitude::to_str() const{
    std::ostringstream oss;
    oss << LatLong::to_str() << ' ' << ((getSign()==Longitude::EAST)?"E":"W");
    return oss.str();
}

std::ostream& operator<<(std::ostream &strm, const Longitude &longitude) {
    return strm << longitude.to_str();
}