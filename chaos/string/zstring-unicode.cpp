/*******************************************************************************
**                                  LibChaos                                  **
**                             zstring-unicode.cpp                            **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zstring.h"
#include "zlog.h"

namespace LibChaos {

bool ZString::isUTF8(const char *str){
    const unsigned char *bytes = (const unsigned char *)str;
    while(*bytes){
        if( // ASCII: use bytes[0] <= 0x7F to allow ASCII control characters
            bytes[0] == 0x09 ||
            bytes[0] == 0x0A ||
            bytes[0] == 0x0D ||
            (0x20 <= bytes[0] && bytes[0] <= 0x7E)
        ){
            bytes += 1;
            continue;
        }

        if( // non-overlong 2-byte
            (0xC2 <= bytes[0] && bytes[0] <= 0xDF) &&
            (0x80 <= bytes[1] && bytes[1] <= 0xBF)
        ){
            bytes += 2;
            continue;
        }

        if(( // excluding overlongs
            bytes[0] == 0xE0 &&
            (0xA0 <= bytes[1] && bytes[1] <= 0xBF) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) || ( // straight 3-byte
                ((0xE1 <= bytes[0] && bytes[0] <= 0xEC) ||
            bytes[0] == 0xEE ||
            bytes[0] == 0xEF) &&
            (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF)
            ) || ( // excluding surrogates
            bytes[0] == 0xED &&
            (0x80 <= bytes[1] && bytes[1] <= 0x9F) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF)
        )){
            bytes += 3;
            continue;
        }

        if(( // planes 1-3
            bytes[0] == 0xF0 &&
            (0x90 <= bytes[1] && bytes[1] <= 0xBF) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
            (0x80 <= bytes[3] && bytes[3] <= 0xBF)
        ) || ( // planes 4-15
            (0xF1 <= bytes[0] && bytes[0] <= 0xF3) &&
            (0x80 <= bytes[1] && bytes[1] <= 0xBF) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
            (0x80 <= bytes[3] && bytes[3] <= 0xBF)
        ) || ( // plane 16
                bytes[0] == 0xF4 &&
            (0x80 <= bytes[1] && bytes[1] <= 0x8F) &&
            (0x80 <= bytes[2] && bytes[2] <= 0xBF) &&
            (0x80 <= bytes[3] && bytes[3] <= 0xBF)
        )){
            bytes += 4;
            continue;
        }
        return false;
    }
    return true;
}

void ZString::unicode_debug(){
    LOG(*this);
    for(zu64 i = 0; i < size(); ++i){
        if((bytes()[i] & 0xF8) == 0xF0){
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            // 4 bytes

            if((bytes()[i+1] & 0xC0) != 0x80)
                LOG("Invalid byte 2: 0x" << ZString::ItoS(bytes()[i+1],16,2));
            if((bytes()[i+2] & 0xC0) != 0x80)
                LOG("Invalid byte 3: 0x" << ZString::ItoS(bytes()[i+2],16,2));
            if((bytes()[i+3] & 0xC0) != 0x80)
                LOG("Invalid byte 4: 0x" << ZString::ItoS(bytes()[i+3],16,2));

            zu32 cp = ((zu32)bytes()[i] & 0x07) << 18 |
                      ((zu32)bytes()[i+1] & 0x3F) << 12 |
                      ((zu32)bytes()[i+2] & 0x3F) << 6 |
                      (bytes()[i+3] & 0x3F);

            LOG(ZString(cc()+i,4) << " (" << codePointStr(cp) << ") [" << ZBinary(bytes()+i, 4).strBytes() << "]");
            i += 3;
        } else if((bytes()[i] & 0xF0) == 0xE0 && (bytes()[i+1] & 0xC0) == 0x80 && (bytes()[i+2] & 0xC0) == 0x80){
            // 1110xxxx 10xxxxxx 10xxxxxx
            // 3 bytes

            if((bytes()[i+1] & 0xC0) != 0x80)
                LOG("Invalid byte 2: 0x" << ZString::ItoS(bytes()[i+1],16,2));
            if((bytes()[i+2] & 0xC0) != 0x80)
                LOG("Invalid byte 3: 0x" << ZString::ItoS(bytes()[i+2],16,2));

            zu32 cp = ((zu32)bytes()[i] & 0x0F) << 12 |
                      ((zu32)bytes()[i+1] & 0x3F) << 6 |
                      (bytes()[i+2] & 0x3F);

            LOG(ZString(cc()+i,3) << " (" << codePointStr(cp) << ") [" << ZBinary(bytes()+i, 3).strBytes() << "]");
            i += 2;
        } else if((bytes()[i] & 0xE0) == 0xC0 && (bytes()[i+1] & 0xC0) == 0x80){
            // 110xxxxx 10xxxxxx
            // 2 bytes

            if((bytes()[i+1] & 0xC0) != 0x80)
                LOG("Invalid byte 2: 0x" << ZString::ItoS(bytes()[i+1],16,2));

            zu32 cp = ((zu32)bytes()[i] & 0x1F) << 6 |
                      (bytes()[i+1] & 0x3F);

            LOG(ZString(cc()+i,2) << " (" << codePointStr(cp) << ") [" << ZBinary(bytes()+i, 2).strBytes() << "]");
            i += 1;
        } else if((bytes()[i] & 0x80) == 0){
            // 0xxxxxxx
            // 1 byte

            zu32 cp = bytes()[i];

            LOG(ZString(cc()+i,1) << " (" << codePointStr(cp) << ") [" << ZBinary(bytes()+i, 1).strBytes() << "]");
        } else {
            LOG("Invalid byte: 0x" << ZString::ItoS(bytes()[i],16,2));
        }
    }
}

ZString ZString::codePointStr(zu64 cp){
    return ZString("U+") + ZString::ItoS(cp,16,8,true);
}

void ZString::unicode_normalize(){

}

}
