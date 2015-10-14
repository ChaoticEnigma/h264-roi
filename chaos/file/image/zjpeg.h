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

#include <jpeglib.h>

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
    ZJPEG(ZImage *image);
    ~ZJPEG();

    static bool isJPEG(const ZBinary &data);

    bool decode(ZReader *input);
    bool encode(ZWriter *output);

public:
    int quality;

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jpeg_error;

private:
    ZImage *_image;
};

}

#endif // ZJPEG_H
