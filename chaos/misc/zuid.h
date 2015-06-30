/*******************************************************************************
**                                  LibChaos                                  **
**                                   zuid.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZUID_H
#define ZUID_H

#include "ztypes.h"
#include "zstring.h"
#include "zbinary.h"

#define ZUID_NIL LibChaos::ZUID(LibChaos::ZUID::nil)

namespace LibChaos {

//! Generates IETF RFC 4122 UUIDs.
class ZUID {
public:
    enum uuidtype {
        //! Nil UUID (00000000-0000-0000-0000-000000000000).
        nil = 0,
        //! Time-based Version 1 UUID.
        time,
        //! Randomly-generated Version 4 UUID.
        random,
        unknown,
    };

public:
    //! Generate new UUID of \a type
    ZUID(uuidtype type = time);
    //! Parse existing UUID
    ZUID(ZString str);

    //! Compare UUIDs
    bool operator==(const ZUID &uid);

    //! Get the type of the UUID
    uuidtype getType() const;

    //! Get hexadecimal UUID string
    ZString str() const;
    //! Get binary container object
    ZBinary bin() const;
    //! Get pointer to raw 16-octet UUID
    const zoctet *raw() const { return _id_octets; }

public:
    //! Get an acceptable MAC address
    static ZBinary getMACAddress();

private:
    //! Check if MAC address is acceptable
    static bool validMAC(const zoctet *addr);

private:
    zoctet _id_octets[16];
};

}

#endif // ZUID_H
