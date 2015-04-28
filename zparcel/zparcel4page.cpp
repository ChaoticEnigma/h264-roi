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
    zassert(buffer.size() != _parser->_pagesize);

    _pagepower = buffer.readzu8();
    _maxpages = buffer.readzu32();
    _nextbackup = buffer.readzu32();
    _freelistpage = buffer.readzu32();
    _fieldlistpage = buffer.readzu32();
}

void HeadPage::save(ZParcel4Page::pageid page){
    _file->setPos(0);
    ZBinary sig = VERSION_4_SIG;
    ZBinary tmp;
    if(_file->write(sig) != SIG_SIZE)
        return false;
    tmp.fromzu8(_pagepower);
    if(_file->write(tmp) != sizeof(zu8))
        return false;
    tmp.fromzu32(_maxpages);
    if(_file->write(tmp) != sizeof(zu32))
        return false;
    tmp.fromzu32(_freelistpage);
    if(_file->write(tmp) != sizeof(zu32))
        return false;
    tmp.fromzu32(_fieldpage);
    if(_file->write(tmp) != sizeof(zu32))
        return false;
    tmp.fromzu32(_indexpage);
    if(_file->write(tmp) != sizeof(zu32))
        return false;
    tmp.fromzu32(_recordpage);
    if(_file->write(tmp) != sizeof(zu32))
        return false;
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
