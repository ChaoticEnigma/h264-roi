#ifndef ZPARCEL4PAGE_H
#define ZPARCEL4PAGE_H

#include "zparcel4parser.h"

#include "zfile.h"
#include "zbinary.h"

#define FREEPAGE        0
#define FIELDPAGE       1
#define FREELISTPAGE    2
#define INDEXPAGE       3
#define RECORDPAGE      4
#define BLOBPAGE        5
#define HISTORYPAGE     6
#define HEADPAGE        80

namespace LibChaos {

class ZParcel4Page {
protected:
    typedef ZParcel4Parser::pageid pageid;
    typedef ZParcel4Parser::pagetype pagetype;

protected:
    ZParcel4Page(ZParcel4Parser *_parser);

public:
    virtual void load(pageid page) = 0;
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
