/*******************************************************************************
**                                  LibChaos                                  **
**                             zstring-unicode.cpp                            **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zstring.h"
#include "zlog.h"

namespace LibChaos {

void ZString::unicode_debug(){
    LOG(*this);
    for(zu64 i = 0; i < size(); ++i){
        if((bytes()[i] & 0xF8) == 0xF0){
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            // 4 bytes
            zu32 cp = ((zu32)bytes()[i] & 0x07) << 18 | ((zu32)bytes()[i+1] & 0x3F) << 12 | ((zu32)bytes()[i+2] & 0x3F) << 6 | (bytes()[i+3] & 0x3F);
            LOG(ZString(cc()+i,1) << " (U+" << ZString::ItoS(cp,16,4) << ")");
            i += 3;
        } else if((bytes()[i] & 0xF0) == 0xE0){
            // 1110xxxx 10xxxxxx 10xxxxxx
            // 3 bytes
            zu32 cp = ((zu32)bytes()[i] & 0x0F) << 12 | ((zu32)bytes()[i+1] & 0x3F) << 6 | (bytes()[i+2] & 0x3F);
            LOG(ZString(cc()+i,1) << " (U+" << ZString::ItoS(cp,16,4) << ")");
            i += 2;
        } else if((bytes()[i] & 0xE0) == 0xC0){
            // 110xxxxx 10xxxxxx
            // 2 bytes
            zu32 cp = ((zu32)bytes()[i] & 0x1F) << 6 | (bytes()[i+1] & 0x3F);
            LOG(ZString(cc()+i,2) << " (U+" << ZString::ItoS(cp,16,4) << ")");
            i += 1;
        } else if((bytes()[i] & 0x80) == 0){
            // 0xxxxxxx
            // 1 byte
            zu32 cp = bytes()[i];
            LOG(ZString(cc()+i,1) << " (U+" << ZString::ItoS(cp,16,4) << ")");
        } else {
            LOG("0x" << ZString::ItoS(bytes()[i], 16, 2));
        }
    }
}

void ZString::unicode_normalize(){

}

}
