/*******************************************************************************
**                                  LibChaos                                  **
**                                  zreader.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZREADER_H
#define ZREADER_H

#include "ztypes.h"

namespace LibChaos {

class ZReader {
public:
    virtual ~ZReader(){}

    /*! Get number of available bytes.
     *  Returns ZU64_MAX for infinity.
     */
    virtual zu64 available() const = 0;

    /*! Read \a size bytes from object at current position into \a dest.
     *  If \a dest is null, call must "read" \a size, but not copy to \a dest.
     *  If \a size if zero, call must have no effect.
     */
    virtual zu64 read(zbyte *dest, zu64 size) = 0;

    // Single Byte
    zu8 readu8(){
        zu8 tmp = 0;
        read(&tmp, 1);
        return tmp;
    }
    zs8 reads8(){
        zs8 tmp = 0;
        read((zu8*)&tmp, 1);
        return tmp;
    }

    // Little-endian Unsigned
    zu16 readleu16(){
        zbyte tmp[2] = {0};
        read(tmp, 2);
        return ((zu16)tmp[0]) |
               ((zu16)tmp[1] << 8);
    }
    zu32 readleu32(){
        zbyte tmp[4] = {0};
        read(tmp, 4);
        return ((zu32)tmp[0]) |
               ((zu32)tmp[1] << 8)  |
               ((zu32)tmp[2] << 16) |
               ((zu32)tmp[3] << 24);
    }
    zu64 readleu64(){
        zbyte tmp[8] = {0};
        read(tmp, 8);
        return ((zu64)tmp[0]) |
               ((zu64)tmp[1] << 8)  |
               ((zu64)tmp[2] << 16) |
               ((zu64)tmp[3] << 24) |
               ((zu64)tmp[4] << 32) |
               ((zu64)tmp[5] << 40) |
               ((zu64)tmp[6] << 48) |
               ((zu64)tmp[7] << 56);
    }

    // Little-endian Signed
    zs16 readles16(){
        zbyte tmp[2] = {0};
        read(tmp, 2);
        return (zs16)(((zu16)tmp[0]) |
               ((zu16)tmp[1] << 8));
    }
    zs32 readles32(){
        zbyte tmp[4] = {0};
        read(tmp, 4);
        return (zs32)(((zu32)tmp[0]) |
               ((zu32)tmp[1] << 8)  |
               ((zu32)tmp[2] << 16) |
               ((zu32)tmp[3] << 24));
    }
    zs64 readles64(){
        zbyte tmp[8] = {0};
        read(tmp, 8);
        return (zs64)(((zu64)tmp[0]) |
               ((zu64)tmp[1] << 8)  |
               ((zu64)tmp[2] << 16) |
               ((zu64)tmp[3] << 24) |
               ((zu64)tmp[4] << 32) |
               ((zu64)tmp[5] << 40) |
               ((zu64)tmp[6] << 48) |
               ((zu64)tmp[7] << 56));
    }

    // Big-endian Unsigned
    zu16 readbeu16(){
        zbyte tmp[2] = {0};
        read(tmp, 2);
        return ((zu16)tmp[0] << 8) |
               ((zu16)tmp[1]);
    }
    zu32 readbeu32(){
        zbyte tmp[4] = {0};
        read(tmp, 4);
        return ((zu32)tmp[0] << 24) |
               ((zu32)tmp[1] << 16) |
               ((zu32)tmp[2] << 8)  |
               ((zu32)tmp[3]);
    }
    zu64 readbeu64(){
        zbyte tmp[8] = {0};
        read(tmp, 8);
        return ((zu64)tmp[0] << 56) |
               ((zu64)tmp[1] << 48) |
               ((zu64)tmp[2] << 40) |
               ((zu64)tmp[3] << 32) |
               ((zu64)tmp[4] << 24) |
               ((zu64)tmp[5] << 16) |
               ((zu64)tmp[6] << 8)  |
               ((zu64)tmp[7]);
    }

    // Big-endian Signed
    zs16 readbes16(){
        zbyte tmp[2] = {0};
        read(tmp, 2);
        return (zs16)(((zu16)tmp[0] << 8) |
               ((zu16)tmp[1]));
    }
    zs32 readbes32(){
        zbyte tmp[4] = {0};
        read(tmp, 4);
        return (zs32)(((zu32)tmp[0] << 24) |
               ((zu32)tmp[1] << 16) |
               ((zu32)tmp[2] << 8)  |
               ((zu32)tmp[3]));
    }
    zs64 readbes64(){
        zbyte tmp[8] = {0};
        read(tmp, 8);
        return (zs64)(((zu64)tmp[0] << 56) |
               ((zu64)tmp[1] << 48) |
               ((zu64)tmp[2] << 40) |
               ((zu64)tmp[3] << 32) |
               ((zu64)tmp[4] << 24) |
               ((zu64)tmp[5] << 16) |
               ((zu64)tmp[6] << 8)  |
               ((zu64)tmp[7]));
    }

    // Floating point
    // Implementation may change
    //! Read single-precision floating point number.
    float readfloat(){
        zbyte tmp[4] = {0};
        read(tmp, 4);
        zu32 raw = ((zu32)tmp[0] << 24) |
                   ((zu32)tmp[1] << 16) |
                   ((zu32)tmp[2] << 8)  |
                   ((zu32)tmp[3]);
        return *(float *)&raw;
    }
    //! Read double-precision floating point number.
    double readdouble(){
        zbyte tmp[8] = {0};
        read(tmp, 8);
        zu64 raw = ((zu64)tmp[0] << 56) |
                   ((zu64)tmp[1] << 48) |
                   ((zu64)tmp[2] << 40) |
                   ((zu64)tmp[3] << 32) |
                   ((zu64)tmp[4] << 24) |
                   ((zu64)tmp[5] << 16) |
                   ((zu64)tmp[6] << 8)  |
                   ((zu64)tmp[7]);
        return *(double *)&raw;
    }
};

}

#endif // ZREADER_H
