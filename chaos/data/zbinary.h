#ifndef ZBINARY_H
#define ZBINARY_H

//#include <vector>

#include "ztypes.h"
#include "zarray.h"

//#include <string.h>

namespace LibChaos {

class ZBinary {
public:
    enum HashType {
        hashType1 = 1
    };

    typedef unsigned char zbinary_type;

    ZBinary();
    ZBinary(const void *ptr, zu64 len);

    bool operator==(const ZBinary &rhs) const {
        if(size() != rhs.size())
            return false;
        return data() == rhs.data();
        //return memcmp(lhs.raw(), rhs.raw(), lhs.size());
    }
    bool operator!=(const ZBinary &rhs) const {
        return !operator==(rhs);
    }


    inline zbinary_type &operator[](zu64 inx){
        return _data[inx];
    }
    inline const zbinary_type &operator[](zu64 inx) const {
        return _data.get(inx);
    }

    ZBinary &resize(zu64 len){
        _data.resize(len);
        return *this;
    }

    ZBinary &fill(zbinary_type dat, zu64 size){
        _data.clear();
        for(zu64 i = 0; i < size; ++i){
            _data.push(dat);
        }
        return *this;
    }

    void concat(const ZBinary &data){
        _data.concat(data.data());
    }

    void reverse(){
        ZArray<zbinary_type> buff;
        for(zu64 i = _data.size(); i > 0; --i){
            buff.push(_data[i]);
        }
        _data = buff;
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

    static zu64 hash(HashType type, const ZBinary &data);
private:
    ZArray<zbinary_type> _data;
};

}

#endif // ZBINARY_H
