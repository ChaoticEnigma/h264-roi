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
    virtual ~YImageBackend(){}

    virtual bool decode(const ZAccessor *input) = 0;
    virtual bool encode(ZWriter *output) = 0;
};

}

#endif // YIMAGEBACKEND

