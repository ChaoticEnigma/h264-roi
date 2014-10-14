#ifndef ZBBB_H
#define ZBBB_H

#include "zstring.h"
using namespace LibChaos;

namespace LibChaosBBB {
    namespace LED {
        const char *usr0 = "/sys/class/leds/beaglebone:green:usr0/brightness";
    }
}

#endif // ZBBB_H
