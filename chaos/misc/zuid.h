#ifndef ZUID_H
#define ZUID_H

#include "ztypes.h"
#include "zstring.h"

namespace LibChaos {

class ZUID {
public:
    struct ID {
        zoctet octets[16];
    };
//    struct ID {
//        union {
//            zoctet octets[16];
//            struct {
//                zu64 first : 64;
//                zu64 second : 64;
//            };
//            struct { // RFC 4122 time-based UUID
//                zu32 time_low                 : 32;
//                zu16 time_mid                 : 16;
//                zu16 time_hi_and_version      : 16;
//                zu8 clock_seq_hi_and_reserved : 8;
//                zu8 clock_seq_low             : 8;
//                zu64 node                     : 48;
//            };
//        };
//    };

public:
    ZUID();
    ZUID(ZString str);

    ID getID() const { return _id; }
    ZString str() const;

private:
    ID _id;
};

}

#endif // ZUID_H
