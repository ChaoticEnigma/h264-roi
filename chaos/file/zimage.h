#ifndef ZIMAGE_H
#define ZIMAGE_H

#include "zbitmap.h"
#include "zpath.h"

namespace LibChaos {

class ZImage {
public:

    //virtual ~ZImage(){}

    virtual bool read(ZPath path) = 0;
    virtual bool write(ZPath path) const = 0;

    //virtual ZBitmap toBitmap() const = 0;
};

}

#endif // ZIMAGE_H
