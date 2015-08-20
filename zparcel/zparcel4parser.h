#ifndef ZPARCEL4PARSER
#define ZPARCEL4PARSER

#include "zfile.h"
#include "zmap.h"
#include "zlist.h"
#include "zuid.h"

#define ZPARCEL_4_NULL      0
#define ZPARCEL_4_UINT      1
#define ZPARCEL_4_SINT      2
#define ZPARCEL_4_ZUID      3
#define ZPARCEL_4_STRING    4
#define ZPARCEL_4_FILE      5
#define ZPARCEL_4_BINARY    6
#define ZPARCEL_4_FLOAT     7
#define ZPARCEL_4_BOOL      8

namespace LibChaos {

class ZParcel4Page;

class ZParcel4Parser {
    friend class ZParcel4Page;
    friend class HeadPage;
    friend class PageTablePage;
    friend class FreeListPage;
    friend class FieldPage;
public:
    typedef zu32 pageid;
    typedef zu8 pagetype;
    typedef zu16 fieldid;
    typedef zu8 fieldtype;

    struct Field {
        fieldid id;
        ZBinary data;
    };

    struct Record {
        Record(ZList<Field> fieldlist) : fields(fieldlist){}
        ZUID uid;
        ZList<Field> fields;
    };

public:
    ZParcel4Parser(ZFile *file);

    //! Create a new parcel, write a new blank head page.
    void create();
    //! Read the head page from the parcel.
    void open();

    zu64 getPageSize() const { return _pagesize; }
    zu64 getMaxPages() const { return _maxpages; }

    void setPageSize(zu8 power);
    void setMaxPages(zu32 pages);

    fieldid addField(ZString name, fieldtype type);
    fieldid getField(ZString name);
    fieldtype getFieldType(fieldid id);

    /*! Add @a record to the parcel.
     *  \return UID of the added record. NIL UUID on failure.
     */
    ZUID addRecord(Record record);
    /*! Add a list of records to the parcel.
     *  \return UIDs of the added records, corresponding to input records. NIL UUID for each failed record.
     */
    ZList<ZUID> addRecords(ZList<Record> records);

    //! Format a boolean value for the parcel.
    static ZBinary formatBool(bool tf);
    //! Format an unsigned integer value for the parcel.
    static ZBinary formatUint(zu64 uint);
    //! Format a signed integer value for the parcel.
    static ZBinary formatSint(zs64 sint);
    //! Format a floating-point (double) value for the parcel.
    static ZBinary formatFloat(double flt);
    //! Format a ZUID for the parcel.
    static ZBinary formatZUID(ZUID uid);
    //! Format text (string) data for the parcel.
    static ZBinary formatString(ZString str);
    //! Format binary blob data for the parcel.
    static ZBinary formatBinary(ZBinary bin);
    //! Format a file reference for the parcel.
    static ZBinary formatFile(ZPath file);

private:
    //! Read page from file.
    void readPage(pageid page, ZBinary &data);
    //! Re-write existing page to file.
    void writePage(pageid page, const ZBinary &data);

    //! Reserve a new page.
    pageid insertPage();

    bool zeroPad();

    //! Mark a page as free and add to the free list.
    //! \note Page should not be reference by other pages.
    bool freePage(pageid page);
    bool addToFreelist(pageid page);

private:
    ZFile *_file;
    bool _init;
    zu64 _pagesize;
    zu64 _maxpages;
    zu64 _pagecount;

    ZParcel4Page *_head;

    ZMap<pageid, ZPointer<ZParcel4Page *> > _pagecache;
};

}

/* ZParcel version 4 format
  A ZParcel 4 file is page-based
  Pages may be in any order, except the head page, which must be the first page
  The page size is the smallest I/O operation that may be performed at a time
  Parser requires at least page size * 2 memory

  Head Page - defines parcel options
  54 bits: parcel signature (ZPARCEL)
   4 bits: parcel category (1)
   4 bits: parcel version (4)
   8 bits: page size power (minimum 5 (32B), default 10 (1KB), maximum 32 (4GB))
  56 bits: maximum number of pages (default 64K)
  32 bits: next head page backup
  64 bits: reserved
  32 bits: first pagetable page number
  32 bits: first freelist page number
  32 bits: first fieldlist page number
  .. zero padding ..


  Pagetable Page - defines page id aliases (for relocated pages)
   8 bits: page type (PAGETABLEPAGETYPE)
  32 bits: next page
  32 bits: previous page
  32 bits: pages in page
  64 bits: alias-page pair
    32 bits: alias id
    32 bits: page id
  .. zero padding..


  Field Page - defines the usable fields in this file
   8 bits: page type (FIELDPAGETYPE)
  32 bits: next page number
  32 bits: previous page number
  16 bits: number of fields on this page
  4+n bytes:
    16 bits: field id
     8 bits: field type
     8 bits: field name length (n)
    8n bits: field name
  .. more fields ..


  Freelist Page - list of pages that are unused and not at the end of the file
   8 bits: page type (FREELISTPAGETYPE)
  32 bits: previous page number
  32 bits: next page number
  32 bits: free page number
  .. more free pages ..

  Free Page
   8 bits: page type (FREEPAGE)
  .. remainder of page is ignored ...


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
