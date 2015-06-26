#ifndef ZOPTIONS_H
#define ZOPTIONS_H

#include "ztypes.h"
#include "zstring.h"
#include "zpath.h"
#include "zfile.h"

namespace LibChaos {

class ZOptions {
public:
    ZOptions();
    ZOptions(ZPath file);
    ~ZOptions();

    ZString getOption(ZString name);
};

}

#endif // ZOPTIONS_H
