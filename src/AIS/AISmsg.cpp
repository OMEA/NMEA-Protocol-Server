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
    unsigned int destPos = 0;
    unsigned char byte=0;
    for(unsigned int i=0;i<deArmoring.length();++i){
        unsigned char corrected = deArmoring[i];
        corrected-=48;
        if(corrected>40){
            corrected-=8;
        }
        if(corrected&0xC0){
            std::cerr << "ERROR wrong coding of AIS"<<std::endl;
        }
        switch(destPos){
            case 0: break;
            case 6: byte |= ((corrected&0x3F)>>4); break;
            case 4: byte |= ((corrected&0x3F)>>2); break;
            case 2: byte |= ((corrected&0x3F)>>0); break;
            default:
                std::cerr << "ERROR THIS SHOULD NEVER HAPPEN!"<<std::endl;
        }
        if(destPos!=0){
            oss << byte;
        }
        switch(destPos){
            case 0: byte = ((corrected&0x3F)<<2); break;
            case 6: byte = ((corrected&0x3F)<<4); break;
            case 4: byte = ((corrected&0x3F)<<6); break;
            case 2: break;
            default:
                std::cerr << "ERROR THIS SHOULD NEVER HAPPEN!"<<std::endl;
        }
        destPos+=6;
        destPos%=8;
    }
    return oss.str();
}

std::string AISmsg::armoring(const std::string armoringStr){
    std::ostringstream oss;
    unsigned char byte=0;
    unsigned int srcPos = 0;
    for(int i=0;i<(int)armoringStr.length();){
        unsigned char source = armoringStr[i];
        switch(srcPos){
            case 0: byte |= ((source>>2)&0x3F); break;
            case 6: byte |= ((source>>4)&0x0F); break;
            case 4: byte |= ((source>>6)&0x03); break;
            case 2: byte |= ((source>>8)&0x00); break;
            default:
                std::cerr << "ERROR THIS SHOULD NEVER HAPPEN!"<<std::endl;
        }
        if(byte&0xC0){
            std::cerr << "ERROR THIS SHOULD NEVER HAPPEN!"<<std::endl;
        }
        if(byte>=40){
            byte+=8;
        }
        byte+=48;
        oss << byte;
        switch(srcPos){
            case 0: byte = ((source<<4)&0x30); ++i; break;
            case 6: byte = ((source<<2)&0x3C); ++i; break;
            case 4: byte = ((source<<0)&0x3F); break;
            case 2: byte = ((source<<8)&0x00); ++i; break;
            default:
                std::cerr << "ERROR THIS SHOULD NEVER HAPPEN!"<<std::endl;
        }
        srcPos+=6;
        srcPos%=8;
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

