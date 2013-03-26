//
//  enums.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#ifndef NMEA_Protocol_Server_enums_h
#define NMEA_Protocol_Server_enums_h

#include <string>
#include <cmath>

enum State {ACTIVE, VOID};



class LatLong{
protected:
    LatLong(){degrees=0; minutes=0;};
public:
    virtual void setDegrees(unsigned short degrees){this->degrees=(degrees%180);}
    unsigned short getDegrees() const {return degrees;}
    double getMinutes() const {return minutes;}
    void setMinutes(double minutes){this->minutes = fmod(minutes,60);}
    
    virtual const std::string to_str() const;
    friend std::ostream& operator<< (std::ostream &out, LatLong &latLong);
private:
    unsigned short degrees;
    double minutes;
};

class Latitude: public LatLong{
public:
    enum Sign {NORTH, SOUTH};
public:
    Latitude(double minutes);
    Latitude():LatLong(){sign=Latitude::NORTH;}
    virtual void setDegrees(unsigned short degrees){LatLong::setDegrees(degrees%90);}
    void setSign(Sign sign){this->sign=sign;}
    unsigned short getSign() const {return sign;}
    
    virtual const std::string to_str() const;
    friend std::ostream& operator<< (std::ostream &out, Latitude &latitude);
private:
    Sign sign;
};

class Longitude: public LatLong{
public:
    enum Sign {EAST, WEST};
public:
    Longitude(double minutes);
    Longitude():LatLong(){sign=Longitude::EAST;}
    void setSign(Sign sign){this->sign=sign;}
    unsigned short getSign() const{return sign;}
    
    virtual const std::string to_str() const;
    friend std::ostream& operator<< (std::ostream &out, Longitude &longitude);
private:
    Sign sign;
};

#endif
