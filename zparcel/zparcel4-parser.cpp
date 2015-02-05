#include "zparcel4-parser.h"

#define ZPARCEL_VERSION_4_SIG { 'P',143,'R',128,144,'L', 1 , 4  }

namespace LibChaos {

ZParcel4Parser::ZParcel4Parser(ZFile *file){
    // Default options
    _pagesize = 9; // 2^9 = 512
}

bool ZParcel4Parser::create(){
    _file->setPos(0);
    // Write file signature
    ZBinary sig = ZPARCEL_VERSION_4_SIG;
    if(_file->write(sig) != ZPARCEL_SIG_SIZE)
        return false;

    if(_file->write(&_pagesize, 1) != 1)
        return false;

    if(_file->write())

    return true;
}

bool ZParcel4Parser::open(){

}


ZString ZParcel4Parser::getName(){

}
void ZParcel4Parser::setName(ZString name){

}

}
