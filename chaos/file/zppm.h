/*******************************************************************************
**                                  LibChaos                                  **
**                                   zppm.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZPPM_H
#define ZPPM_H

#include "zimage.h"
#include "yimagebackend.h"
#include "zpath.h"

namespace LibChaos {

class ZPPM : public YImageBackend {
public:
    ZPPM(ZImage *image) : _image(image){}

    static bool isPPM(const ZBinary &data);

    bool decode(ZBinary &ppmdata_in, ReadOptions *options = nullptr);
    bool encode(ZBinary &ppmdata_out, WriteOptions *options = nullptr);

    bool read(ZPath path);
    bool write(ZPath path);

private:
    ZImage *_image;
};

}

#endif // ZPPM_H
