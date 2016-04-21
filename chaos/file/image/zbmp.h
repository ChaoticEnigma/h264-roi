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

//! BMP encoder/decoder.
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

    static bool isBMP(const ZBinary &data);

    bool decode(ZReader *input);
    bool encode(ZWriter *output);

private:
    /*! Converts BMP-encoded pixel data to RGB-encoded pixel data.
     *  Allocates a buffer to hold the RGB data.
     */
    unsigned char *convertBMPDatatoRGB(const unsigned char *bmpbuffer, zu32 width, zu32 height);
    /*! Converts RGB-encoded pixel data to BMP-encoded pixel data.
     *  Allocates a buffer to hold the BMP data.
     */
    unsigned char *convertRGBtoBMPData(const unsigned char *rgbbuffer, zu32 width, zu32 height, zu64 &outsize);

private:
    ZImage *_image;
    const zu8 bmp_channels = 3;
};

}

#endif // ZBMP_H
