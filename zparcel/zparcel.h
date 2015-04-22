#ifndef ZPARCEL_H
#define ZPARCEL_H

#include "zstring.h"
#include "zpath.h"
#include "zassoc.h"
#include "zbinary.h"
#include "zjson.h"
#include "zfile.h"

#include "zparcel4parser.h"

namespace LibChaos {

class ZParcelSection : public ZReader {
public:
    ZParcelSection(ZFile *file, zu64 start, zu64 length) : file(file), start(start), readpos(0), length(length){}

    bool isOpen(){
        return file->isOpen();
    }

    // ZReader interface
    zu64 read(zbyte *dest, zu64 size){
        size = MIN(size, length - readpos);
        if(setPos(readpos) != readpos)
            return 0;
        zu64 read = file->read(dest, size);
        readpos += read;
        return read;
    }
    bool atEnd() const {
        return getPos() >= length;
    }
    zu64 setPos(zu64 pos){
        pos = MIN(length, pos);
        readpos = file->setPos(start + pos) - start;
        return readpos;
    }
    zu64 getPos() const {
        return file->getPos() - start;
    }

private:
    ZFile *file;
    zu64 start;
    zu64 readpos;
    zu64 length;
};

class ZParcel {
public:
    enum parceltype {
        versionunknown = 0,
//        version2,
//        version3,
        version4 = 4,
    };

    struct ParcelSection {
        zu64 pos;
        zu64 len;
        ZPath file;
        bool istmp;
    };
    typedef ZAssoc<ZString, ParcelSection> ParcelSectionList;

public:
    ZParcel();
    ~ZParcel();

    // Create new parcel (will overwrite existing parcel)
    bool create(ZPath path);
    // Open existing parcel
    bool open(ZPath file);
    // Test parcel signature
    static bool testParcel(ZPath file);
    // Close file handles
    void close();

private:
    // Parcel file
    ZPath _path;
    ZFile _file;
    parceltype _version;
    ZParcel4Parser *_parser;
};

} // namespace LibChaos

#endif // ZPARCEL_H
