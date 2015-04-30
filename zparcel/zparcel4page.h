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
#define FIELDPAGETYPE       0x01
#define FREELISTPAGETYPE    0x02
#define INDEXPAGETYPE       0x03
#define RECORDPAGETYPE      0x04
#define BLOBPAGETYPE        0x05
#define HISTORYPAGETYPE     0x06
#define PAGETABLEPAGETYPE   0x07

namespace LibChaos {

class ZParcel4Page {
protected:
    typedef ZParcel4Parser::pageid pageid;
    typedef ZParcel4Parser::pagetype pagetype;

protected:
    ZParcel4Page(ZParcel4Parser *_parser);

public:
    //! Parse existing page in file.
    virtual void load(pageid page) = 0;

    //! Create a new page with default data.
    virtual void init() = 0;

    //! Save page to file.
    virtual void save() = 0;

    virtual pagetype type() = 0;

protected:
    ZParcel4Parser *_parser;
    pageid _page;
};


class HeadPage : public ZParcel4Page {
public:
    HeadPage(ZParcel4Parser *_parser);
    void load(pageid page);
    void init();
    void save();
    pagetype type(){ return HEADPAGETYPE; }

    zu8 _pagepower;
    zu32 _maxpages;
    pageid _nextbackup;
    pageid _pagetablepage;
    pageid _freelistpage;
    pageid _fieldlistpage;
};


class FieldPage : public ZParcel4Page {
public:
    FieldPage(ZParcel4Parser *_parser);
    void load(pageid page);
    void init();
    void save();
    pagetype type(){ return FIELDPAGETYPE; }

    pagetype _pagetype;
    pageid _prevpage;
    pageid _nextpage;
};

}

#endif // ZPARCEL4PAGE_H
