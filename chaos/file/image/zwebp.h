#ifndef ZWEBP_H
#define ZWEBP_H

#include "zimage.h"
#include "yimagebackend.h"

namespace LibChaos {

class ZWebP : public YImageBackend {
public:
    ZWebP();

    bool decode(ZBinary &data_in, ReadOptions *options);
    bool encode(ZBinary &data_out, WriteOptions *options);
};

}

#endif // ZWEBP_H
