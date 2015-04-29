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

void HeadPage::load(pageid page){
    ZBinary buffer;
    _parser->readPage(page, buffer);
    zu64 sig = buffer.readzu64();
    if(sig != VERSION_4_MASK)
        throw ZException("HeadPage load: incorrect head page signature");
    _pagepower = buffer.readzu8();
    _maxpages = buffer.readzu32();
    _nextbackup = buffer.readzu32();
    _pagetablepage = buffer.readzu32();
    _freelistpage = buffer.readzu32();
    _fieldlistpage = buffer.readzu32();
}

void HeadPage::save(ZParcel4Page::pageid page){
    ZBinary buffer;
    buffer.writezu64(VERSION_4_MASK);
    buffer.writezu8(_pagepower);
    buffer.writezu32(_maxpages);
    buffer.writezu32(_nextbackup);
    buffer.writezu32(_pagetablepage);
    buffer.writezu32(_freelistpage);
    buffer.writezu32(_fieldlistpage);
    _parser->writePage(page, buffer);
}

// /////////////////////////////////////////
// FieldPage
// /////////////////////////////////////////

FieldPage::FieldPage(ZParcel4Parser *parser) : ZParcel4Page(parser){

}

void FieldPage::load(pageid page){
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

void FieldPage::save(ZParcel4Page::pageid page){

}

}
