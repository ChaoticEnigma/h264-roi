#ifndef ZPARCEL4PAGE_H
#define ZPARCEL4PAGE_H

#include "zparcel4parser.h"

#include "zfile.h"
#include "zbinary.h"

#define VERSION_4_SIG { 90,80,143,82,128,144,76,65 } // 0x5A508F5280904C41 // ZPARCEL 4 1
#define VERSION_4_MASK 0x5A508F5280904C41 // ZPARCEL // Version 4, Type 1
#define SIG_SIZE 8

#define HEADPAGETYPE        80
#define FREEPAGETYPE        0
#define FIELDPAGETYPE       1
#define FREELISTPAGETYPE    2
#define INDEXPAGETYPE       3
#define RECORDPAGETYPE      4
#define BLOBPAGETYPE        5
#define HISTORYPAGETYPE     6
#define PAGETABLEPAGETYPE   7

namespace LibChaos {

class ZParcel4Page {
protected:
    typedef ZParcel4Parser::pageid pageid;
    typedef ZParcel4Parser::pagetype pagetype;

protected:
    ZParcel4Page(ZParcel4Parser *_parser);

public:
    //! Parse existing page in file
    virtual void load(pageid page) = 0;
    //!
    virtual void save(pageid page) = 0;

protected:
    ZParcel4Parser *_parser;
    pageid _page;
};


class HeadPage : public ZParcel4Page {
public:
    HeadPage(ZParcel4Parser *_parser);
    void load(pageid page);
    void save(pageid page);

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
    void save(pageid page);

    pagetype _pagetype;
    pageid _prevpage;
    pageid _nextpage;
};

}

#endif // ZPARCEL4PAGE_H
