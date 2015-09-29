/*******************************************************************************
**                                  LibChaos                                  **
**                               yimagebackend.h                              **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef YIMAGEBACKEND
#define YIMAGEBACKEND

#include "ztypes.h"
#include "zbinary.h"

namespace LibChaos {

class YImageBackend {
public:
    virtual bool decode(ZBinary &data_in) = 0;
    virtual bool encode(ZBinary &data_out, void *options = nullptr) = 0;
};

}

#endif // YIMAGEBACKEND

