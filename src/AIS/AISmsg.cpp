//
//  AISmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#include <sstream>

#include "AISmsg.h"
#include "PositionReportmsg.h"

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
uint64_t ubits(const char buf[], unsigned int start, unsigned int width, bool le)
/* extract a (zero-origin) bitfield from the buffer as an unsigned big-endian uint64_t */
{
    uint64_t fld = 0;
    unsigned int i;
    unsigned end;
    
    /*@i1@*/ assert(width <= sizeof(uint64_t) * CHAR_BIT);
    for (i = start / CHAR_BIT;
         i < (start + width + CHAR_BIT - 1) / CHAR_BIT; i++) {
        /*@i1@*/fld <<= CHAR_BIT;
        fld |= (unsigned char)buf[i];
    }
    
    end = (start + width) % CHAR_BIT;
    if (end != 0) {
        /*@i1@*/fld >>= (CHAR_BIT - end);
    }
    
    /*@ -shiftimplementation @*/
    fld &= ~(-1LL << width);
    /*@ +shiftimplementation @*/
    
    /* was extraction as a little-endian requested? */
    if (le)
    {
        uint64_t reversed = 0;
        
        for (i = width; i; --i)
        {
            reversed <<= 1;
            if (fld & 1)
                reversed |= 1;
            fld >>= 1;
        }
        fld = reversed;
    }
    
    return fld;
}

int64_t sbits(const char buf[], unsigned int start, unsigned int width, bool le)
/* extract a bitfield from the buffer as a signed big-endian long */
{
    uint64_t fld = ubits((const char *)buf, start, width, le);
    
    /*@ +relaxtypes */
    if (fld & (1LL << (width - 1))) {
        /*@ -shiftimplementation @*/
        fld |= (-1LL << (width - 1));
        /*@ +shiftimplementation @*/
    }
    return (int64_t)fld;
    /*@ -relaxtypes */
}

std::string AISmsg::deArmoring(const std::string deArmoring){
    std::ostringstream oss;
    unsigned char byte=0;
    unsigned int destPos = 0;
    for(unsigned int i=0;i<deArmoring.length();++i){
        int shift = (2-destPos);
        unsigned char corrected = deArmoring[i];
        corrected-=48;
        if(corrected>40){
            corrected-=8;
        }
        if(corrected&0xC0){
            std::cerr << "ERROR wrong coding of AIS"<<std::endl;
        }
        if(shift>=0){
            byte |= ((corrected&0x3F)<<shift);
        }
        else{
            byte |= ((corrected&0x3F)>>abs(shift));
        }
        destPos+=6;
        
        if(destPos>=8){
            destPos-=8;
            oss << byte;
            if(destPos!=0){
                byte = ((corrected&0x3F)<<(8-destPos));
            }
        }
    }

    
    return oss.str();
}

std::string AISmsg::armoring(const std::string armoringStr){
    std::ostringstream oss;
    unsigned char byte=0;
    unsigned int srcPos = 2;
    bool notStart = false;
    for(int i=0;i<(int)armoringStr.length();){
        //if(srcPos!=2){
        unsigned char origin = armoringStr[i];
        if(notStart){
            byte = (origin<<(8-srcPos))&0x3F;
        }
        notStart=true;
        //}
        //else{
        //    byte = 0;
        //}
        if(srcPos!=6){
            i++;
        }
        //if(srcPos!=8){
            byte |= (origin>>srcPos)&0x3F;
        //std::cout << "i=" << i << "pos=" << srcPos << "byte=" << (unsigned int)byte << "stringbyte=" << (unsigned int)origin <<std::endl;
        //}
        if(byte&0xC0){
            std::cerr << "ERROR wrong coding of AIS has bit1 or bit2 set!"<<std::endl;
        }
        if(byte>=40){
            byte+=8;
        }
        byte+=48;
        oss << byte;
        srcPos+=2;
        if(srcPos>8)
            srcPos-=8;
        
    }
    return oss.str();
}


boost::shared_ptr<AISmsg> AISmsg::factory(std::string parseMsg) {
    boost::shared_ptr<AISmsg> ret;
    std::string deArm = AISmsg::deArmoring(parseMsg);
    unsigned char type = ubits(deArm.c_str(), 0, 6, false);
    switch(type){
        case AISmsg::POSITION_REPORT_CLASS_A:
        case AISmsg::POSITION_REPORT_CLASS_A_2:
        case AISmsg::POSITION_REPORT_CLASS_A_3:
            ret = PositionReportmsg::factory(deArm);
            break;
        default:
            ret = boost::shared_ptr<AISmsg>(new AISmsg(deArm));
    }
    return ret;
}

AISmsg::AISmsg(std::string parseMsg){
    type = (AISmsg::MsgType)ubits(parseMsg.c_str(), 0, 6, false);
    repeat = ubits(parseMsg.c_str(), 6, 2, false);
    mmsi = ubits(parseMsg.c_str(), 8, 30, false);
}

