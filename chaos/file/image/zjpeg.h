/*******************************************************************************
**                                  LibChaos                                  **
**                                   zjpeg.h                                  **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZJPEG_H
#define ZJPEG_H

#include "zimage.h"
#include "yimagebackend.h"
#include "zbinary.h"

namespace LibChaos {

class ZJPEG : public YImageBackend {
public:
    struct JPEGError {
        enum jpegerrors {
            none = 0,
            badreadfile = 1,
            badwritefile = 2
        };
    };
    struct JPEGWrite {
        enum jpegoptions {
            none = 0,
        };
    };

public:
    ZJPEG(ZImage *image) : _image(image){}

    static bool isJPEG(const ZBinary &data);

    bool decode(ZBinary &jpegdata_in, ReadOptions *options = nullptr);
    bool encode(ZBinary &jepgdata_out, WriteOptions *options = nullptr);

private:
    ZImage *_image;
};

}

#endif // ZJPEG_H
