#ifndef YIMAGEBACKEND
#define YIMAGEBACKEND

#include "ztypes.h"
#include "zbinary.h"

namespace LibChaos {

class YImageBackend {
public:
    bool decode(ZBinary &data_in);
    bool encode(ZBinary &data_out, void *options = nullptr);
};

}

#endif // YIMAGEBACKEND

