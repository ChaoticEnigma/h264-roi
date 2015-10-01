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
    struct ReadOptions {

    };
    struct WriteOptions {

    };

public:
    virtual ~YImageBackend(){}

    virtual bool decode(ZBinary &data_in, ReadOptions *options = nullptr) = 0;
    virtual bool encode(ZBinary &data_out, WriteOptions *options = nullptr) = 0;
};

}

#endif // YIMAGEBACKEND

