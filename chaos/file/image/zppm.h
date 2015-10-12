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

    bool decode(ZReader *input);
    bool encode(ZWriter *output);

private:
    ZImage *_image;
};

}

#endif // ZPPM_H
