/*******************************************************************************
**                                  LibChaos                                  **
**                                   zppm.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZPPM_H
#define ZPPM_H

#include "zimage.h"
#include "zpath.h"

namespace LibChaos {

class ZPPM {
public:
    ZPPM(){

    }
    ZPPM(const ZImage &img){
        if(img.isRGB24())
            image = img;
    }

    bool read(ZPath path);
    bool write(ZPath path);

private:
    ZImage image;
};

}

#endif // ZPPM_H
