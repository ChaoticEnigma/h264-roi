#ifndef ZPARCEL_H
#define ZPARCEL_H

#include "zstring.h"
#include "zpath.h"
#include "zassoc.h"
#include "zbinary.h"
#include "zjson.h"
#include "zfile.h"

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
        version2,
        version3,
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
    bool create(ZPath file);
    // Open existing parcel
    bool open(ZPath file);
    // Test parcel signature
    bool testParcel(ZPath file) const;
    // Close file handles
    void close();

    bool addFile(ZString name, ZPath file);
    bool addData(ZString name, const ZBinary &data);

    bool getSection(ParcelSection location, ZBinary &out);
    ZParcelSection readSection(ZString name);

    ZString keyByIndex(zu64 index);

    bool getByName(ZString name, ZBinary &out);
    ZBinary operator[](ZString name);

    ZBinary getByIndex(zu64 index);
    //inline ZBinary operator[](zu64 inx){ return getByIndex(inx); }

    // Parcel creation
    static zu64 makeParcel(ZPath out, ZAssoc<ZString, ZPath> in);

    zu64 finishParcel();

    // Extract all parcelled files to a folder
    zu64 unParcel();

    zu64 parcelContSize();

    inline ZPath parcelFile() const { return _path; }
    inline ParcelSectionList getIndex() const { return _index; }
    inline zu64 size() const { return _index.size(); }

private:
    ZPath tmpParcelPath();

    parceltype readSig();
    bool writeSig();

private:
    // Parcel file
    ZPath _path;
    ZFile _parcel;
//    ZFile _tmpfl;

    // Parcel index
    ParcelSectionList _index;

    // Read
    parceltype _type;
    zu64 _headsize;

    // Write
    zu64 currpos;
};

typedef ZParcel::ParcelSection PcSc;
typedef ZParcel::ParcelSectionList PcScLs;
typedef ZParcel::ParcelSection PclSec;
typedef ZParcel::ParcelSectionList PclSecLst;

} // namespace LibChaos

#endif // ZPARCEL_H
