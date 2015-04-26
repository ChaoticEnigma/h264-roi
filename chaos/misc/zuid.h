/*******************************************************************************
**                                  LibChaos                                  **
**                                   zuid.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZUID_H
#define ZUID_H

#include "ztypes.h"
#include "zstring.h"

#define ZUID_NIL LibChaos::ZUID(LibChaos::ZUID::nil)

namespace LibChaos {

//! Generates IETF RFC 4122 UUIDs
class ZUID {
public:
    enum uuidtype {
        //! Nil UUID (00000000-0000-0000-0000-000000000000)
        nil = 0,
        //! Time-based Version 1 UUID
        time,
        //! Randomly-generated Version 4 UUID
        random,
    };

public:
    //! Generate new UUID of type
    //! Default time-based UUID
    ZUID(uuidtype type = time);
    //! Parse existing UUID
    ZUID(ZString str);

    //! Compare UUIDs
    bool operator==(const ZUID &uid);

    //! Get hexadecimal UUID string
    ZString str() const;
    //! Get pointer to raw 16-octet UUID
    const zoctet *raw() const { return _id_octets; }

private:
    zoctet _id_octets[16];
};

}

#endif // ZUID_H
