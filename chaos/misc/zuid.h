/*******************************************************************************
**                                  LibChaos                                  **
**                                   zuid.h                                   **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZUID_H
#define ZUID_H

#include "ztypes.h"
#include "zstring.h"
#include "zbinary.h"
#include "zlist.h"

#define ZUID_NIL LibChaos::ZUID(LibChaos::ZUID::NIL)

namespace LibChaos {

//! Generates IETF RFC 4122 UUIDs.
class ZUID {
public:
    enum uuidtype {
        //! Nil UUID (00000000-0000-0000-0000-000000000000).
        NIL = 0,
        //! Time-based Version 1 UUID.
        TIME,
        //! Randomly-generated Version 4 UUID.
        RANDOM,
        //! Error value.
        UNKNOWN,
    };

public:
    //! Generate new UUID of \a type.
    ZUID(uuidtype type = TIME);
    /*! Parse existing UUID string.
     *  String must contain 32 hexadecimal characters,
     *  ignoring any number of '-' or ':' characters in string.
     */
    ZUID(ZString str);

    //! Compare UUIDs.
    bool operator==(const ZUID &uid);

    //! Get the type of the UUID.
    uuidtype getType() const;

    //! Get hexadecimal UUID string.
    ZString str(bool separate = true, ZString delim = "-") const;
    //! Get binary container object.
    ZBinary bin() const;
    //! Get pointer to raw 16-octet UUID.
    const zoctet *raw() const { return _id_octets; }

public:
    //! Get an acceptable timestamp.
    static zu64 getTimestamp();
    //! Get all MAC addresses.
    static ZList<ZBinary> getMACAddresses();
    //! Get an acceptable MAC address.
    static ZBinary getMACAddress();

private:
    //! Check if MAC address is acceptable.
    static bool validMAC(const zoctet *addr);

private:
    //! UUID octets.
    zoctet _id_octets[16];
};

}

#endif // ZUID_H
