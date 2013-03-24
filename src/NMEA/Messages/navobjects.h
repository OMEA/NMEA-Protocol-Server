//
//  enums.h
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#ifndef NMEA_Protocol_Server_enums_h
#define NMEA_Protocol_Server_enums_h

enum State {ACTIVE, VOID};



class LatLong{
public:
    virtual void setDegrees(unsigned short degrees){this->degrees=(degrees%180);}
    unsigned short getDegrees(){return degrees;}
    void setMinutes(double minutes){this->minutes=fmod(minutes,60);}
    double getMinutes(){return minutes;}
private:
    unsigned short degrees;
    double minutes;
};

class Latitude: public LatLong{
public:
    enum Sign {NORTH, SOUTH};
public:
    void setDegrees(unsigned short degrees){LatLong::setDegrees(degrees%90);}
    void setSign(Sign sign){this->sign=sign;}
    unsigned short getSign(){return sign;}
private:
    Sign sign;
};

class Longitude: public LatLong{
public:
    enum Sign {EAST, WEST};
public:
    void setSign(Sign sign){this->sign=sign;}
    unsigned short getSign(){return sign;}
private:
    Sign sign;
};

#endif
