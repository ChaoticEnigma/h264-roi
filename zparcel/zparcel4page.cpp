#include "zparcel4page.h"

#include "zlog.h"

namespace LibChaos {

// /////////////////////////////////////////
// ParcelPage
// /////////////////////////////////////////

ZParcel4Page::ZParcel4Page(ZParcel4Parser *parser) : _parser(parser){

}

// /////////////////////////////////////////
// HeadPage
// /////////////////////////////////////////

HeadPage::HeadPage(ZParcel4Parser *parser) : ZParcel4Page(parser){

}

// /////////////////////////////////////////
// FieldPage
// /////////////////////////////////////////

FieldPage::FieldPage(ZParcel4Parser *parser) : ZParcel4Page(parser){

}

void FieldPage::load(){
//    ZBinary buff;
//    buff.resize(1);
//    _parser->_file->read(buff.raw(), 1);
//    if(buff.tozu8() != FIELDPAGE){
//        ELOG("FieldPage created on wrong page type");
//        return;
//    }
//    buff.resize(4);
//    read(buff.raw(), 4);
//    _prevpage = buff.tozu32();
//    setPos(_pagesize - 4);
//    read(buff.raw(), 4);
//    _nextpage = buff.tozu32();
}

}
