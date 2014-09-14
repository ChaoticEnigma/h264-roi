#ifndef ZJPEG_H
#define ZJPEG_H

#include "zimage.h"
#include "zbinary.h"
#include "zpath.h"
#include "zerror.h"

namespace LibChaos {

class ZJPEG {
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
    ZJPEG(){

    }
    ZJPEG(const ZImage &img) : image(img){

    }

    bool decode(ZBinary &jpegdata_in);
    bool encode(ZBinary &jepgdata_out, JPEGWrite::jpegoptions options = JPEGWrite::none);

    bool read(ZPath path);
    bool write(ZPath path, JPEGWrite::jpegoptions options = JPEGWrite::none);

    ZImage &getImage(){
        return image;
    }

private:
    ZImage image;
    ZError error;
};

}

#endif // ZJPEG_H
