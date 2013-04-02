//
//  AISmsg.cpp
//  NMEA-Protocol-Server
//
//  Created by Till Steinbach on 21.03.13.
//
//

#include <sstream>
#include <stdexcept>

#include "AISmsg.h"
#include "PositionReportmsg.h"
#include "StaticVoyageRelatedmsg.h"
#include "PositionReportClassBmsg.h"

const char AISmsg::sixBit2Char[64] = {'@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_',' ','!','\"','#','$','%','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?'};
const std::map<char,unsigned char> AISmsg::char2SixBit = boost::assign::map_list_of('@',0)('A',1)('B',2)('C',3)('D',4)('E',5)('F',6)('G',7)('H',8)('I',9)('J',10)('K',11)('L',12)('M',13)('N',14)('O',15)('P',16)('Q',17)('R',18)('S',19)('T',20)('U',21)('V',22)('W',23)('X',24)('Y',25)('Z',26)('[',27)('\\',28)(']',29)('^',30)('_',31)(' ',32)('!',33)('\"',34)('#',35)('#',36)('%',37)('&',38)('\'',39)('(',40)(')',41)('*',42)('+',43)(',',44)('-',45)('.',46)('/',47)('0',48)('1',49)('2',50)('3',51)('4',52)('5',53)('6',54)('7',55)('8',56)('9',57)(':',58)(';',59)('<',60)('=',61)('>',62)('?',63);



unsigned long getUBits(const std::string from, unsigned int offset, unsigned int length){
    if(length>(sizeof(unsigned long)*8)){
        throw std::invalid_argument("unsigned long not large enough!");
    }
    unsigned long retVal=0;
    unsigned int end = offset+length-1;
    unsigned int tmpLength=length;
    for(unsigned int i=(offset/8); i<=(end/8); ++i){
        if(i>=from.length()){
                throw std::invalid_argument("Accessed bits behind string end!");
        }
        unsigned long byte = (unsigned char)from[i];
        int shift = tmpLength+(offset%8)-8;
        if(shift>0){
            byte<<=shift;
        }
        else{
            byte>>=abs(shift);
        }
        retVal|=byte;
        tmpLength-=8;
    }
    retVal&= ~(-1LL << length);
    return retVal;
}

void setUBits(std::string &to, unsigned int offset, unsigned int length, unsigned long value){
    if(length>(sizeof(unsigned long)*8)){
        throw std::invalid_argument("unsigned long not large enough!");
    }
    unsigned int end = offset+length-1;
    unsigned int tmpLength=length;
    
    value&= ~(-1LL << length);
    for(unsigned int i=(offset/8); i<=(end/8); ++i){
        if(i>=to.length()){
            throw std::invalid_argument("Accessed bits behind string end!");
        }
        int shift = tmpLength+(offset%8)-8;
        unsigned char byte;
        if(shift>0){
            byte=value>>shift;
        }
        else{
            byte=value<<abs(shift);
        }
        byte|=(unsigned char)to[i];
        char sbyte = (char)byte;
        to.replace(i,1,1,sbyte);
        tmpLength-=8;
    }
}

long getSBits(const std::string from, unsigned int offset, unsigned int length){
    unsigned long bits = getUBits(from, offset, length);
    //if most significant bit is one (negative number), set all upper bits one
    if (bits & (1LL << (length - 1))) {
        bits |= (-1LL << (length));
    }
    return bits;
}

void setSBits(std::string &to, unsigned int offset, unsigned int length, long value){
    setUBits(to, offset, length, (unsigned long)value);
}

std::string getStringBits(const std::string from, unsigned int offset, unsigned int length){
    if(length%6){
        throw std::invalid_argument("length does not match a number of six bit characters");
    }
    std::string retVal;
    while(length>0){
        char appendChar = AISmsg::sixBit2Char[getUBits(from, offset, 6)];
        if(appendChar=='@'){
            break;
        }
        retVal.append(1,appendChar);
        offset+=6;
        length-=6;
    }
    return retVal;
}

void setStringBits(std::string &to, unsigned int offset, unsigned int length, std::string str){
    if(length%6){
        throw std::invalid_argument("length does not match a number of six bit characters");
    }
    std::string retVal;
    unsigned int i=0;
    while(length>0){
        char mappedChar = '@';
        if(i<str.length()){
            std::map<char,unsigned char>::const_iterator it = AISmsg::char2SixBit.find(str[i]);
            if(it!=AISmsg::char2SixBit.end()){
                mappedChar = (*it).second;
            }
            else{
                throw std::invalid_argument("invalid character in string");
            }
        }
        else{
            mappedChar = '@';
        }
        setUBits(to, offset, 6, mappedChar);
        offset+=6;
        length-=6;
        ++i;
    }
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
    unsigned char type = getUBits(deArm, 0, 6);
    switch(type){
        case AISmsg::POSITION_REPORT_CLASS_A:
        case AISmsg::POSITION_REPORT_CLASS_A_2:
        case AISmsg::POSITION_REPORT_CLASS_A_3:
            ret = PositionReportmsg::factory(deArm);
            break;
        case AISmsg::STATIC_VOYAGE_RELATED:
            ret = StaticVoyageRelatedmsg::factory(deArm);
            break;
        case AISmsg::STANDARD_CLASS_B_CS_POSITION_REPORT:
            ret = PositionReportClassBmsg::factory(deArm);
            break;
        default:
            ret = boost::shared_ptr<AISmsg>(new AISmsg(deArm));
    }
    return ret;
}

AISmsg::AISmsg(unsigned int mmsi){
    setBitLength(38);
    type = AISmsg::UNDEFINED;
    repeat = 0;
    this->mmsi = mmsi;
}

AISmsg::AISmsg(std::string parseMsg){
    setBitLength(38);
    type = (AISmsg::MsgType)getUBits(parseMsg, 0, 6);
    repeat = getUBits(parseMsg, 6, 2);
    mmsi = getUBits(parseMsg, 8, 30);
}

std::string AISmsg::toCodedStr(){
    std::string(retVal);
    code(retVal);
    return AISmsg::armoring(retVal);
}

void AISmsg::code(std::string &code){
    unsigned int length=code.length();
    unsigned int required=(((getBitLength()-1)/8)+1);
    if(length<required){
        code.append(required-length, 0);
    }
    setUBits(code,0,6,(unsigned char)type);
    setUBits(code,6,2,(unsigned char)repeat);
    setUBits(code,8,30,mmsi);
}