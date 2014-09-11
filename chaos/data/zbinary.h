#ifndef ZBINARY_H
#define ZBINARY_H

#include "ztypes.h"
#include "zarray.h"
#include <string.h>

namespace LibChaos {

class ZBinary {
public:
    typedef unsigned char zbinary_type;

    enum HashType {
        hashType1 = 1
    };

public:
    class RAW {
        static void *fillRaw(void *dest, const void *src, zu64 src_size, zu64 dest_count){
            for(zu64 i = 0; i < dest_count; ++i){
                memcpy((unsigned char *)dest + i, src, src_size);
            }
            return dest;
        }
    };


public:
    ZBinary() : _data(nullptr), _size(0){

    }
    ZBinary(const void *ptr, zu64 size) : ZBinary(){
        if(ptr && size){
            _size = size;
            _data = new zbinary_type[_size];
            memcpy(_data, ptr, _size);
        }
    }
    ZBinary(const ZBinary &other) : ZBinary(other._data, other._size){

    }

    ~ZBinary(){
        clear();
    }

    ZBinary &operator=(const ZBinary &other){
        clear();
        if(other._data && other._size){
            _size = other._size;
            _data = new zbinary_type[_size];
            memcpy(_data, other._data, _size);
        }
        return *this;
    }

    bool operator==(const ZBinary &rhs) const {
        if(_size != rhs._size)
            return false;
        return memcmp(_data, rhs._data, _size);
    }
    bool operator!=(const ZBinary &rhs) const {
        return !operator==(rhs);
    }

    zbinary_type &operator[](zu64 inx){
        return _data[inx];
    }
    const zbinary_type &operator[](zu64 inx) const {
        return _data[inx];
    }

    ZBinary &resize(zu64 size){
        zbinary_type *tmp = new zbinary_type[size];
        if(_data && tmp && _size && size)
            memcpy(tmp, _data, MIN(_size, size));
        _size = size;
        delete[] _data;
        _data = tmp;
        return *this;
    }

    ZBinary &fill(zbinary_type dat, zu64 size){
        clear();
        _size = size;
        _data = new zbinary_type[_size];
        memset(_data, dat, _size);
        return *this;
    }

    ZBinary &concat(const ZBinary &other){
        zu64 old = _size;
        resize(_size + other._size);
        memcpy(_data + old, other._data, other._size);
        return *this;
    }

    void reverse(){
        zbinary_type *buff = new zbinary_type[_size];
        for(zu64 i = 0, j = _size; i <_size; ++i, --j){
            buff[j] = _data[i];
        }
        delete[] _data;
        _data = buff;
    }

    void clear(){
        _size = 0;
        delete[] _data;
        _data = nullptr;
    }

    zu64 size() const{
        return _size;
    }

    zbinary_type *raw(){
        return _data;
    }
    const zbinary_type *raw() const {
        return _data;
    }

    static zu64 hash(HashType type, const ZBinary &data);
private:
    zbinary_type *_data;
    zu64 _size;
};

}

#endif // ZBINARY_H
