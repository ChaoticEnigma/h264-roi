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
    if(buffer.readbeu64() != VERSION_4_MASK)
        throw ZException("HeadPage load: incorrect head page signature");
    _pagepower = buffer.readu8();
    _maxpages = buffer.readbeu32();
    _nextbackup = buffer.readbeu32();
    _pagetablepage = buffer.readbeu32();
    _freelistpage = buffer.readbeu32();
    _fieldpage = buffer.readbeu32();
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
    buffer.writebeu64(VERSION_4_MASK);
    buffer.writeu8(_pagepower);
    buffer.writebeu32(_maxpages);
    buffer.writebeu32(_nextbackup);
    buffer.writebeu32(_pagetablepage);
    buffer.writebeu32(_freelistpage);
    buffer.writebeu32(_fieldpage);
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
    _nextpage = buffer.readbeu32();
    _prevpage = buffer.readbeu32();
    zu64 pages = buffer.readbeu32();
    if(pages * 4 > (_parser->_pagesize - 12))
        throw ZException("FreeListPage load: Invalid number of pages in page");
    for(zu64 i = 0 ; i < pages; ++i){
        zu64 pid = buffer.readbeu32();
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
    buffer.writebeu32(_nextpage);
    buffer.writebeu32(_prevpage);
    buffer.writebeu32(_pages.size());
    for(zu64 i = 0; i < _pages.size(); ++i){
        buffer.writebeu32(_pages[i]);
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
