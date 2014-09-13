#ifndef ZJPEG_H
#define ZJPEG_H

#include "zimage.h"
#include "zpath.h"
#include "zerror.h"

namespace LibChaos {

class ZJPEG {
public:
    struct JPEGError {
        enum jpegerrors {
            none = 0,
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
