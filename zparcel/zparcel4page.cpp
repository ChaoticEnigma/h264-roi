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
    _fieldpage = buffer.readzu32();
}

void HeadPage::init(){
    _page = 0;
    _pagepower = DEFAULT_PAGE_SIZE;
    _maxpages = DEFAULT_MAX_PAGES;
    _nextbackup = 0;
    _pagetablepage = 0;
    _freelistpage = 0;
    _fieldpage = 0;
}

void HeadPage::save(){
    ZBinary buffer;
    buffer.writezu64(VERSION_4_MASK);
    buffer.writezu8(_pagepower);
    buffer.writezu32(_maxpages);
    buffer.writezu32(_nextbackup);
    buffer.writezu32(_pagetablepage);
    buffer.writezu32(_freelistpage);
    buffer.writezu32(_fieldpage);
    _parser->writePage(_page, buffer);
}


// /////////////////////////////////////////
// PageTablePage
// /////////////////////////////////////////

PageTablePage::PageTablePage(ZParcel4Parser *parser) : ZParcel4Page(parser){

}

void PageTablePage::load(ZParcel4Page::pageid page){
    _page = page;
    ZBinary buffer;
    _parser->readPage(_page, buffer);

}

void PageTablePage::init(){
    _page = _parser->insertPage();

}

void PageTablePage::save(){
    ZBinary buffer;

    _parser->writePage(_page, buffer);
}


// /////////////////////////////////////////
// FreeListPage
// /////////////////////////////////////////

FreeListPage::FreeListPage(ZParcel4Parser *parcel) : ZParcel4Page(parcel){

}

void FreeListPage::load(ZParcel4Page::pageid page){
    _page = page;
    ZBinary buffer;
    _parser->readPage(_page, buffer);
    _nextpage = buffer.readzu32();
    _prevpage = buffer.readzu32();
    zu64 pages = buffer.readzu32();
    if(pages * 4 > (_parser->_pagesize - 12))
        throw ZException("FreeListPage load: Invalid number of pages in page");
    for(zu64 i = 0 ; i < pages; ++i){
        zu64 pid = buffer.readzu32();
        if(pid != 0){
            _pages.push(pid);
        }
    }
}

void FreeListPage::init(){
    _page = _parser->insertPage();
    _nextpage = 0;
    _prevpage = 0;
}

void FreeListPage::save(){
    ZBinary buffer;
    buffer.writezu32(_nextpage);
    buffer.writezu32(_prevpage);
    buffer.writezu32(_pages.size());
    for(zu64 i = 0; i < _pages.size(); ++i){
        buffer.writezu32(_pages[i]);
    }
    _parser->writePage(_page, buffer);
}


// /////////////////////////////////////////
// FieldPage
// /////////////////////////////////////////

FieldPage::FieldPage(ZParcel4Parser *parser) : ZParcel4Page(parser){

}

void FieldPage::load(pageid page){
    _page = page;
    ZBinary buffer;
    _parser->readPage(_page, buffer);

}

void FieldPage::init(){
    _page = _parser->insertPage();

}

void FieldPage::save(){
    ZBinary buffer;

    _parser->writePage(_page, buffer);
}

}
