#ifndef ZPARCEL4PARSER
#define ZPARCEL4PARSER

#include "zparcel-parser.h"

namespace LibChaos {

class ZParcel4Parser : public ZParcelParser {
public:
    ZParcel4Parser(ZFile *file);

    bool create();
    bool open();

    void setPageSize(zu8 size){ _pagesize = size; }
    void setMaxPages(zu32 pages){ _maxpages = pages; }

    bool commit();

    ZString getName();
    void setName(ZString name);

private:
    void writeHeadPage();

private:
    ZFile *_file;
    zu8 _pagesize;
    zu32 _maxpages;
    zu32 _pagecount;
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
  32 bits: tables page number
  32 bits: freelist page number (0 if none)
  .. zero padding ..


  Table Page - defines the tables and their columns in this file
   8 bits: page type id (1)
  32 bits: previous page number
  16 bits: number of tables
  16 bits: table id
  32 bits: first index page
  32 bits: first record page
   8 bits: table name length (n)
   n bits: table name
  16 bits: number of columns
   8 bits: column type id
   8 bits: column name length (m)
   m bits: column name
  .. more columns ..
  .. more tables ..
  32 bits: next page number


  Freelist Page - list of pages that are unused and not at the end of the file
   8 bits: page type id (2)
  32 bits: previous page number (0 if none)
  32 bits: free page number
  .. more free pages ..
  32 bits: next page number (0 if none)

  Free Page
   8 bits: page type id(0)
  .. zeroes ...


  Index Page -


  Record Page - contains table rows
   8 bits: page type id (3)
  32 bits: previous page number (0 if none)
   n bits: record
   m bits: column
  .. more columns ..
  .. more records ..
  32 bits: next page number (0 if none)


  Blob Page
   8 bits: page type id (4)
  32 bits: previous page number (0 if none)
   n bits: blob data
  32 bits: next page number (0 if none)


  History Page
   8 bits: page type id (5)


*/

#endif // ZPARCEL4PARSER
