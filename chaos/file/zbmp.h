#ifndef ZBMP_H
#define ZBMP_H

#include "zimage.h"
#include "zpath.h"

#define BI_RGB 0x0000

namespace LibChaos {

class ZBMP : public ZImage {
public:
    ZBMP(){

    }
    ZBMP(const ZBitmap &image) : bitmap(image){

    }

    bool read(ZPath path);
    bool write(ZPath path) const;

    ZBitmap toBitmap() const {
        return bitmap;
    }

private:
    ZBitmap bitmap;
};

}

#endif // ZBMP_H
