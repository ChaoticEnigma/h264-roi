#ifndef ZPARCEL4PARSER
#define ZPARCEL4PARSER

#include "zparcel-parser.h"

namespace LibChaos {

namespace ZParcelTypes {

typedef zu32 pageid;
enum pagetype {
    freepage,
    fieldpage,
    freelistpage,
    indexpage,
    recordpage,
    blobpage,
    historypage,
    headpage,
};

typedef zu16 fieldid;
enum fieldtype {
    nullfield,
    unsignedintfield,
    signedintfield,
    zuidfield,
    stringfield,
    filefield,
};

struct Field {
    fieldid id;
    ZBinary data;
};

typedef ZArray<Field> FieldList;

}

class ZParcel4Parser : public ZParcelParser {
public:
    ZParcel4Parser(ZFile *file);

    bool create();
    bool open();

    zu32 getPageSize() const { return _pagesize; }
    zu32 getMaxPages() const { return _maxpages; }

    void setPageSize(zu8 power);
    void setMaxPages(zu32 pages);

    ZParcelTypes::fieldid addField(ZString name, ZParcelTypes::fieldtype type);
    ZParcelTypes::fieldid getFieldId(ZString name);
    ZParcelTypes::fieldtype getFieldType(ZParcelTypes::fieldid id);

    bool addRecord(ZParcelTypes::FieldList fields);

    static ZParcelTypes::fieldtype fieldFileIdToFieldType(zu16 type);
    static ZString getFieldTypeName(ZParcelTypes::fieldtype type);

private:
    bool readPage(ZParcelTypes::pageid page, ZBinary &data);
    bool loadHeadPage();

    ZParcelTypes::pageid insertPage(ZParcelTypes::pagetype type);
    bool writeHeadPage();

    bool zeroPad();

    bool freePage(ZParcelTypes::pageid page);
    bool addToFreelist(ZParcelTypes::pageid page);

private:
    ZFile *_file;
    bool _init;
    zu32 _pagesize;
    zu32 _pagecount;

    zu8 _pagepower;
    zu32 _maxpages;
    ZParcelTypes::pageid _freelistpage;
    ZParcelTypes::pageid _fieldpage;
    ZParcelTypes::pageid _indexpage;
    ZParcelTypes::pageid _recordpage;
};

class ParcelPage : public ZWriter, public ZReader, public ZPosition {
public:
    ParcelPage(ZFile *file, zu32 page, zu32 pagesize);

    zu64 read(zbyte *dest, zu64 size);
    zu64 write(const zbyte *src, zu64 size);

    zu64 setPos(zu64 pos){ _rwpos = pos; return _rwpos; }
    zu64 getPos() const { return _rwpos; }
    bool atEnd() const;

protected:
    ZFile *_file;
    ZParcelTypes::pageid _page;
    zu32 _pagesize;
    zu64 _rwpos;
};

class FieldPage : public ParcelPage {
public:
    FieldPage(ZFile *file, zu32 page, zu32 pagesize);



    ZParcelTypes::pagetype _pagetype;
    ZParcelTypes::pageid _prevpage;
    ZParcelTypes::pageid _nextpage;
};

namespace ZParcelConvert {
    ZBinary toFileFormat(ZParcelTypes::fieldtype type, ZString str);

    ZBinary toFile8Bits(zu8 num);
    ZBinary toFile16Bits(zu16 num);
    ZBinary toFile32Bits(zu32 num);
    ZBinary toFile64Bits(zu64 num);
    zu8 fromFile8Bits(ZBinary num);
    zu16 fromFile16Bits(ZBinary num);
    zu32 fromFile32Bits(ZBinary num);
    zu64 fromFile64Bits(ZBinary num);
}

}

/* ZParcel version 4 format
  A ZParcel 4 file is page-based
  Pages may be in any order, except the head page, which must be the first page
  The page size is the smallest I/O operation that may be performed at a time

  Head Page - defines parcel options
  48 bits: parcel signature
   8 bits: parcel category (1)
   8 bits: parcel version (4)
   8 bits: page size power (minimum 5 (32), default 10 (1K), maximum 32 (4G))
  32 bits: maximum number of pages (default 64K)
  32 bits: freelist page number (0 if none)
  32 bits: field page number
  32 bits: index page number
  32 bits: record page number
  .. zero padding ..


  Field Page - defines the usable fields in this file
   8 bits: page type
  32 bits: previous page number
  16 bits: number of fields
  16 bits: field id
   8 bits: field type
   8 bits: field name length (n)
   n bits: field name
  .. more fields ..
  32 bits: next page number


  Freelist Page - list of pages that are unused and not at the end of the file
   8 bits: page type
  32 bits: previous page number (0 if none)
  32 bits: free page number
  .. more free pages ..
  32 bits: next page number (0 if none)

  Free Page
   8 bits: page type
  .. zeroes ...


  Index Page - shortlist of record locations
   8 bits: page type
  32 bits: previous page number
  16 bits: field id
   n bits: field data
  32 bits: page number
  32 bits: page byte
  .. more record locations ..
  32 bits: next page number


  Record Page - contains records
   8 bits: page type
  32 bits: previous page number (0 if none)
  16 bits: number of fields in record
  16 bits: field id
   n bits: field data
  .. more fields ..
  .. more records ..
  32 bits: next page number (0 if none)


  Blob Page
   8 bits: page type
  32 bits: previous page number (0 if none)
   n bits: blob data
  32 bits: next page number (0 if none)


  History Page
   8 bits: page type


  Page Types
   0: free page
   1: field page
   2: freelist page
   3: index page
   4: record page
   5: blob page
   6: history page
  80: head page

  Field Types
   0:
   1:

*/

#endif // ZPARCEL4PARSER
