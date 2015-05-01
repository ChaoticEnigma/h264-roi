#ifndef ZPARCEL4PAGE_H
#define ZPARCEL4PAGE_H

#include "zparcel4parser.h"

#include "zfile.h"
#include "zbinary.h"

#define VERSION_4_SIG { 90,80,143,82,128,144,76,65 } // 0x5A508F5280904C41 // ZPARCEL 4 1
#define VERSION_4_MASK (zu64)0x5A508F5280904C41ull // ZPARCEL // Version 4, Type 1
#define SIG_SIZE 8

#define DEFAULT_PAGE_SIZE 10 // 2 ^ 10 = 1024 bytes
#define DEFAULT_MAX_PAGES (64 * 1024) // 65536 pages

#define FREEPAGE            0x00
#define RESERVEDPAGE        0xFF
#define HEADPAGETYPE        0x5A
#define FIELDPAGE           0x01
#define FREELISTPAGE        0x02
#define INDEXPAGETYPE       0x03
#define RECORDPAGETYPE      0x04
#define BLOBPAGETYPE        0x05
#define HISTORYPAGETYPE     0x06
#define PAGETABLEPAGE       0x07

namespace LibChaos {

class ZParcel4Page {
protected:
    typedef ZParcel4Parser::pageid pageid;
    typedef ZParcel4Parser::pagetype pagetype;
    typedef ZParcel4Parser::fieldid fieldid;
    typedef ZParcel4Parser::fieldtype fieldtype;

protected:
    ZParcel4Page(ZParcel4Parser *parser);

public:
    //! Parse existing page in file.
    virtual void load(pageid page) = 0;

    //! Create a new page with default data.
    virtual void init() = 0;

    //! Save page to file.
    virtual void save() = 0;

    virtual pagetype type() = 0;

    pageid page() const { return _page; }

protected:
    ZParcel4Parser *_parser;
    pageid _page;
};

//! The head page contains information about the parcel.
//! Contains locations of important pages.
class HeadPage : public ZParcel4Page {
public:
    HeadPage(ZParcel4Parser *parser);
    void load(pageid page);
    void init();
    void save();
    pagetype type(){ return HEADPAGETYPE; }

    zu8 _pagepower;
    zu64 _maxpages;
    pageid _nextbackup;
    pageid _pagetablepage;
    pageid _freelistpage;
    pageid _fieldpage;
};

//! A page table page translates page ids to page locations.
//! Only pages that have different ids and locations go in the page table,
//! usually pages that have been moved since they were created.
class PageTablePage : public ZParcel4Page {
public:
    struct PagePair {
        pageid id;
        pageid position;
    };
public:
    PageTablePage(ZParcel4Parser *parser);
    void load(pageid page);
    void init();
    void save();
    pagetype type(){ return PAGETABLEPAGE; }

    pageid _nextpage;
    pageid _prevpage;
    ZMap<PagePair> _table;
};

//! Contains a list of page locations that have been deleted.
//! New pages can replace free pages, rather than making the file larger.
class FreeListPage : public ZParcel4Page {
public:
    FreeListPage(ZParcel4Parser *parcel);
    void load(pageid page);
    void init();
    void save();
    pagetype type(){ return FREELISTPAGE; }

    pageid _nextpage;
    pageid _prevpage;
    ZArray<pageid> _pages;
};

//! Defines fields that can contain recrod data.
class FieldPage : public ZParcel4Page {
public:
    struct Field {
        fieldid id;
        fieldtype type;
        zu8 options;
        ZString name;
    };
public:
    FieldPage(ZParcel4Parser *parser);
    void load(pageid page);
    void init();
    void save();
    pagetype type(){ return FIELDPAGE; }

    pageid _nextpage;
    pageid _prevpage;
    zu16 _pagefields;
    ZArray<Field> _fields;
};

}

#endif // ZPARCEL4PAGE_H
