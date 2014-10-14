#ifndef ZBMP_H
#define ZBMP_H

#include "zimage.h"
#include "zpath.h"
#include "zerror.h"

#define BI_RGB 0x0000

namespace LibChaos {

class ZBMP {
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
    ZBMP(const ZImage &img) : image(img){

    }

    bool read(ZPath path);
    bool write(ZPath path);

    ZError getError(){
        ZError err = error;
        error.clear();
        return err;
    }

    ZImage &getImage(){
        return image;
    }

private:
    unsigned char *convertBMPDatatoRGB(unsigned char *bmpbuffer, zu32 height, zu32 width);
    unsigned char *convertRGBtoBMPData(unsigned char *rgbbuffer, zu32 height, zu32 width, zu64 &outsize);

private:
    ZImage image;
    ZError error;

    const zu8 bmp_channels = 3;
};

}

#endif // ZBMP_H
