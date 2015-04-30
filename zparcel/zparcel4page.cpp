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
    _page = page;
    ZBinary buffer;
    _parser->readPage(_page, buffer);
    if(buffer.readzu64() != VERSION_4_MASK)
        throw ZException("HeadPage load: incorrect head page signature");
    _pagepower = buffer.readzu8();
    _maxpages = buffer.readzu32();
    _nextbackup = buffer.readzu32();
    _pagetablepage = buffer.readzu32();
    _freelistpage = buffer.readzu32();
    _fieldlistpage = buffer.readzu32();
}

void HeadPage::init(){
    _page = 0;
    _pagepower = DEFAULT_PAGE_SIZE;
    _maxpages = DEFAULT_MAX_PAGES;
    _nextbackup = 0;
    _pagetablepage = 0;
    _freelistpage = 0;
    _fieldlistpage = 0;
}

void HeadPage::save(){
    ZBinary buffer;
    buffer.writezu64(VERSION_4_MASK);
    buffer.writezu8(_pagepower);
    buffer.writezu32(_maxpages);
    buffer.writezu32(_nextbackup);
    buffer.writezu32(_pagetablepage);
    buffer.writezu32(_freelistpage);
    buffer.writezu32(_fieldlistpage);
    _parser->writePage(_page, buffer);
}

// /////////////////////////////////////////
// FieldPage
// /////////////////////////////////////////

FieldPage::FieldPage(ZParcel4Parser *parser) : ZParcel4Page(parser){

}

void FieldPage::load(pageid page){
    _page = page;

}

void FieldPage::init(){
    _page = _parser->insertPage();
}

void FieldPage::save(){

}

}
