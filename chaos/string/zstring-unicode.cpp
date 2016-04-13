/*******************************************************************************
**                                  LibChaos                                  **
**                             zstring-unicode.cpp                            **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zstring.h"
#include "zlog.h"

namespace LibChaos {

void ZString::unicode_debug(){
    for(zu64 i = 0; i < size(); ++i){
        LOG(at(i) << " (U+" << ZString::ItoS(at(i),16,4) << ")");
    }
}

void ZString::unicode_normalize(){

}

}
