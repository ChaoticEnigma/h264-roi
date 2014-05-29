#ifndef ZBINARY_H
#define ZBINARY_H

//#include <vector>

#include "ztypes.h"
#include "zarray.h"

#include <string.h>

namespace LibChaos {

class ZBinary {
public:
    typedef unsigned char zbinary_type;

    ZBinary();
    ZBinary(const zbinary_type *ptr, zu64 len);

    inline zbinary_type &operator[](zu64 inx){
        return _data[inx];
    }
    inline const zbinary_type &operator[](zu64 inx) const {
        return _data[inx];
    }

    inline ZBinary &fill(zbinary_type dat, zu64 size){
        _data.clear();
        for(zu64 i = 0; i < size; ++i){
            _data.push(dat);
        }
        return *this;
    }

    zu64 size() const{
        return _data.size();
    }

    ZArray<zbinary_type> &data(){
        return _data;
    }
    const ZArray<zbinary_type> &data() const {
        return _data;
    }

    const zbinary_type *raw() const {
        return _data.ptr();
    }

    static zu64 hash(char type, const ZBinary &data);
private:
    ZArray<zbinary_type> _data;
};

inline bool operator==(const ZBinary &lhs, const ZBinary &rhs){
    if(lhs.size() != rhs.size())
        return false;
    return lhs.data() == rhs.data();
    //return memcmp(lhs.raw(), rhs.raw(), lhs.size());
}
inline bool operator!=(const ZBinary &lhs, const ZBinary &rhs){
    return !operator==(lhs, rhs);
}

}

#endif // ZBINARY_H
