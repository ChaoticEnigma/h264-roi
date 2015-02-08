#include "zparcel4-parser.h"
#include "zlog.h"

#define VERSION_4_SIG { 'P',143,'R',128,144,'L', 1 , 4 }
#define DEFAULT_MAX_PAGES (64 * 1024)

namespace LibChaos {

ZParcel4Parser::ZParcel4Parser(ZFile *file){
    // Default options
    _pagesize = 9; // 2^9 = 512
}

bool ZParcel4Parser::create(){

    return true;
}

bool ZParcel4Parser::open(){

}

ZParcel4Parser::fieldid ZParcel4Parser::addField(ZString name, ZParcel4Parser::fieldtype type){

}

ZParcel4Parser::fieldid ZParcel4Parser::getFieldId(ZString name){

}

bool ZParcel4Parser::addRecord(ZParcel4Parser::FieldList fields){

}

bool ZParcel4Parser::readPage(pageid page, ZBinary &data){
    _file->setPos(page * _pagesize);
    return (_file->read(data, _pagesize) == _pagesize);
}

pageid ZParcel4Parser::insertPage(ZParcel4Parser::pagetype type){
    switch(type){
        case headpage:
            if(writeHeadPage())
                return 0;
            ELOG("insertPage failed to write head page");
            break;
        case fieldpage:
            break;
        case freelistpage:
            break;
        case indexpage:
            break;
        case recordpage:
            break;
        case blobpage:
            break;
        case historypage:
            break;
        case freepage:
            break;
        default:
            ELOG("insertPage unknown page type");
            break;
    }
}

bool ZParcel4Parser::writeHeadPage(){
    _file->setPos(0);
    ZBinary sig = VERSION_4_SIG;
    if(_file->write(sig) != ZPARCEL_SIG_SIZE)
        return false;
    if(_file->write(toFile8Bits(_pagesize)) != sizeof(zu8))
        return false;
    if(_file->write(toFile32Bits(_maxpages)) != sizeof(zu32))
        return false;
    if(_file->write(toFile32Bits(_fieldpage)) != sizeof(zu32))
        return false;
    if(_file->write(toFile32Bits(_freelistpage)) != sizeof(zu32))
        return false;

    return true;
}

bool ZParcel4Parser::freePage(pageid page){
    _file->setPos(page * _pagesize);
    if(_file->write({0}) == 1){
        if(addToFreelist(page))
            return true;
    }
    return false;
}

bool ZParcel4Parser::addToFreelist(pageid page){

    return false;
}

ZBinary ZParcel4Parser::toFile8Bits(zu8 num){
    ZBinary bin(1);
    bin[0] = num;
    return bin;
}

ZBinary ZParcel4Parser::toFile16Bits(zu16 num){
    ZBinary bin(2);
    bin[0] = (zu8)((num >> 8) & 0x00FF);
    bin[1] = (zu8)((num)      & 0x00FF);
    return bin;
}

ZBinary ZParcel4Parser::toFile32Bits(zu32 num){
    ZBinary bin(1);
    bin[0] = (zu8)((num >> 24) & 0x000000FF);
    bin[1] = (zu8)((num >> 16) & 0x000000FF);
    bin[2] = (zu8)((num >> 8)  & 0x000000FF);
    bin[3] = (zu8)((num)       & 0x000000FF);
    return bin;
}

}
