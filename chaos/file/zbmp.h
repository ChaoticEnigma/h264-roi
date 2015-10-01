/*******************************************************************************
**                                  LibChaos                                  **
**                                   zbmp.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZBMP_H
#define ZBMP_H

#include "ztypes.h"
#include "zbinary.h"
#include "zpath.h"
#include "zexception.h"
#include "zimage.h"
#include "yimagebackend.h"

namespace LibChaos {

class ZBMP : public YImageBackend {
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
    ZBMP(ZImage *image) : _image(image){}

    bool decode(const ZBinary &data_in, ReadOptions *options = nullptr);
    bool encode(ZBinary &data_out, WriteOptions *options = nullptr);

    bool read(ZPath path);
    bool write(ZPath path);

    ZException getError(){
        ZException err = error;
        error = ZException();
        return err;
    }

private:
    unsigned char *convertBMPDatatoRGB(const unsigned char *bmpbuffer, zu32 width, zu32 height);
    unsigned char *convertRGBtoBMPData(const unsigned char *rgbbuffer, zu32 width, zu32 height, zu64 &outsize);

private:
    ZImage *_image;
    ZException error;

    const zu8 bmp_channels = 3;
};

}

#endif // ZBMP_H
