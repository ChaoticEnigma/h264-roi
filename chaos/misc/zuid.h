#ifndef ZUID_H
#define ZUID_H

#include "ztypes.h"
#include "zstring.h"

#define ZUID_NIL LibChaos::ZUID(LibChaos::ZUID::nil)

namespace LibChaos {

// Generates IETF RFC 4122 UUIDs
class ZUID {
public:
    enum uuidtype {
        nil = 0,
        time,
        random,
    };

public:
    ZUID(uuidtype type = time);
    ZUID(ZString str);

    bool operator==(const ZUID &uid);

    const zoctet *getID() const { return _id_octets; }
    ZString str() const;

private:
    zoctet _id_octets[16];
};

}

#endif // ZUID_H
