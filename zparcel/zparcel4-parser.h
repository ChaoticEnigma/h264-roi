#ifndef ZPARCEL4PARSER
#define ZPARCEL4PARSER

#include "zparcel-parser.h"

namespace LibChaos {

class ZParcel4Parser : public ZParcelParser {
public:
    ZParcel4Parser(ZFile *file);

    bool create();
    bool open();


    ZString getName();
    void setName(ZString name);

private:
    ZFile *_file;
    zu8 _pagesize;
    zu32 _maxpages;
    zu32 _pagecount;
};

}

/* ZParcel version 4 format

  Head Page (once at beginning of file)
  48 bits: parcel signature
   8 bits: parcel category (1)
   8 bits: parcel version (4)
   8 bits: page size power (minimum 512, default 1K)
  32 bits: maximum number of pages
   8 bits: table name length (n)
   n bits: table name
  .. remaining bits to next page are 0 ..

  Record Page


  Blob Page


  History Page


*/

#endif // ZPARCEL4PARSER

