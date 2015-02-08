#ifndef ZPARCEL4PARSER
#define ZPARCEL4PARSER

#include "zparcel-parser.h"

namespace LibChaos {

class ZParcel4Parser : public ZParcelParser {
public:
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

    };

    struct Field {
        fieldid id;
        ZBinary data;
    };

    typedef ZArray<Field> FieldList;

public:
    ZParcel4Parser(ZFile *file);

    bool create();
    bool open();

    void setPageSize(zu8 size){ _pagesize = size; }
    void setMaxPages(zu32 pages){ _maxpages = pages; }

    fieldid addField(ZString name, fieldtype type);
    fieldid getFieldId(ZString name);

    bool addRecord(FieldList fields);

private:
    bool readPage(pageid page, ZBinary &data);

    pageid insertPage(pagetype type);
    bool writeHeadPage();

    bool freePage(pageid page);
    bool addToFreelist(pageid page);

    static ZBinary toFile8Bits(zu8 num);
    static ZBinary toFile16Bits(zu16 num);
    static ZBinary toFile32Bits(zu32 num);

private:
    ZFile *_file;
    zu8 _pagesize;
    zu32 _maxpages;
    zu32 _pagecount;
    pageid _fieldpage;
    pageid _freelistpage;
};

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
  32 bits: field page number
  32 bits: freelist page number (0 if none)
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
