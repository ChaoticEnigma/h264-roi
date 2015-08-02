/*******************************************************************************
**                                  LibChaos                                  **
**                                zautobuffer.h                               **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZAUTOBUFFER_H
#define ZAUTOBUFFER_H

#include "zautobufferunit.h"
#include "zfile.h"

namespace LibChaos {

class ZAutoBuffer {
public:
    ZAutoBuffer();
    ~ZAutoBuffer();

    ZAutoBuffer &add(ZString str);
    ZAutoBuffer &add(ZString str, zu64 pos, zu64 len);
    ZAutoBuffer &add(char *ptr, zu64 len);
    ZAutoBuffer &add(char *ptr, zu64 pos, zu64 len);
    ZAutoBuffer &add(const char *ptr, zu64 len);
    ZAutoBuffer &add(const char *ptr, zu64 pos, zu64 len);
    ZAutoBuffer &add(ZFile file);
    ZAutoBuffer &add(ZFile file, zu64 pos, zu64 len);
    ZAutoBuffer &add(zu64 num);
private:
    enum ref_type {
        type_string = 1,
        type_sub_string = 2,
        type_ptr = 3,
        type_const_ptr = 4,
        type_file = 5,
        type_sub_file = 6,
        type_integer = 7
    };
    struct ref_set {
        ref_type type;
        const char *ptr;
        zu64 ptr_len;
        zu64 len;
        zu64 num;
    };

    ZArray<ref_set> refs;
};

}

#endif // ZAUTOBUFFER_H
