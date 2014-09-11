#ifndef ZBMP_H
#define ZBMP_H

#include "zimage.h"
#include "zpath.h"

#define BI_RGB 0x0000

namespace LibChaos {

class ZBMP : public ZImage {
public:
    struct BMPError {
        enum {
            none = 0,
            notabmp = 1,
            badfile = 2,
            badinfoheader = 3,
            badcompression = 4,
            incorrectsize = 5,
            incorrectdimensions = 6,
            badbitcount = 7
        };
    };
public:
    ZBMP(){

    }
    ZBMP(const ZBitmap &image) : bitmap(image){

    }

    bool read(ZPath path);
    bool write(ZPath path) const;

    ZError getError(){
        ZError err = error;
        error.clear();
        return err;
    }

    ZBitmap toBitmap() const {
        return bitmap;
    }

private:
    ZBitmap bitmap;
    ZError error;
};

}

#endif // ZBMP_H
