#ifndef ZPARCELPARSER
#define ZPARCELPARSER

#include "ztypes.h"
#include "zfile.h"

#define ZPARCEL_SIG_SIZE 8

namespace LibChaos {

class ZParcelParser {
public:
//    ZParcelParser(ZFile *file);

    virtual bool create() = 0;
    virtual bool open() = 0;

    virtual ZString getName() = 0;
    virtual void setName(ZString name) = 0;
};

}

#endif // ZPARCELPARSER
