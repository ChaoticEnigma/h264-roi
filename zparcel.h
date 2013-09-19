#ifndef ZPARCEL_H
#define ZPARCEL_H

#include "zstring.h"
#include "zpath.h"
#include "zlog.h"
#include "zfile.h"

namespace LibChaos {

class ZParcel {
public:
    struct ParcelSection {
        ParcelSection(){}
        ParcelSection(unsigned long p, unsigned long l) : pos(p), len(l){}
        unsigned long pos;
        unsigned long len;
    };
    typedef AssocArray<ParcelSection> ParcelSectionList;

    ZParcel(ZPath);
    static bool makeParcel(ZPath out, AssocArray<ZPath> in);
    static ParcelSectionList readIndex(ZPath);
    ParcelSectionList getIndex();
    ZString getSection(ParcelSection);
private:
    bool ready;
    ZPath parcel;
    ParcelSectionList index;
    unsigned long ioffset;
};

typedef ZParcel::ParcelSection PcSc;
typedef ZParcel::ParcelSectionList PcScLs;

} // namespace LibChaos

#endif // ZPARCEL_H
