#ifndef ZBINARY_H
#define ZBINARY_H

#include "ztypes.h"
#include "zarray.h"
#include <string.h>
#include "zreader.h"
#include "zstorage.h"

namespace LibChaos {

namespace ZBinaryInternal {

template <class S = ZDefaultStorage> class ZBinary : public ZReader {
public:

    static_assert(std::is_base_of<ZStorage, S>::value, "ZBinary template parameter is not derived from ZStorage");

    typedef unsigned char zbinary_type;

    enum HashType {
        hashType1 = 1
    };

    static const zu64 none = (zu64)-1;

public:
    class RAW {
    public:
        static void *fillRaw(void *dest, const void *src, zu64 src_size, zu64 dest_count){
            for(zu64 i = 0; i < dest_count; ++i){
                memcpy((unsigned char *)dest + i, src, src_size);
            }
            return dest;
        }
    };


public:
    ZBinary() : _data(nullptr), _size(0), _stor(new S){

    }
    ZBinary(const void *ptr, zu64 size) : ZBinary(){
        if(ptr && size){
            _size = size;
            _data = new zbinary_type[_size];
            memcpy(_data, ptr, _size);
        }
    }
    ZBinary(ZArray<zbinary_type> arr) : ZBinary(){
        if(arr.size()){
            _size = arr.size();
            _data = new zbinary_type[_size];
            memcpy(_data, arr.ptr(), _size);
        }
    }
    ZBinary(std::initializer_list<zbinary_type> list) : ZBinary(){
        resize(list.size());
        zu64 i = 0;
        for(auto it = list.begin(); it < list.end(); ++it, ++i){
            _data[i] = *it;
        }
    }
    ZBinary(const ZBinary<> &other) : ZBinary(other._data, other._size){

    }

    ~ZBinary(){
        delete[] _data;
        _data = nullptr;
    }
    void clear(){
        _size = 0;
        delete[] _data;
        _data = nullptr;
    }

    ZBinary<> &operator=(const ZBinary<> &other){
        clear();
        if(other._data && other._size){
            _size = other._size;
            _data = new zbinary_type[_size];
            memcpy(_data, other._data, _size);
        }
        return *this;
    }

    bool operator==(const ZBinary<> &rhs) const {
        if(_size != rhs._size)
            return false;
        return memcmp(_data, rhs._data, _size);
    }
    bool operator!=(const ZBinary<> &rhs) const {
        return !operator==(rhs);
    }

    zbinary_type &operator[](zu64 inx){
        return _data[inx];
    }
    const zbinary_type &operator[](zu64 inx) const {
        return _data[inx];
    }

    // Reading interface
    zu64 read(unsigned char *dest, zu64 length){
        if(_readerpos + length > _size){
            length = _readerpos + length - _size;
        }
        if(dest && length)
            memcpy(dest, _data + _readerpos, length);
        _readerpos += length;
        return length;
    }
    bool atEnd() const {
        return _readerpos == _size;
    }

    ZBinary<> &resize(zu64 size){
        zbinary_type *tmp = new zbinary_type[size];
        if(_data && tmp && _size && size)
            memcpy(tmp, _data, MIN(_size, size));
        _size = size;
        delete[] _data;
        _data = tmp;
        return *this;
    }

    ZBinary<> &fill(zbinary_type dat, zu64 size){
        clear();
        _size = size;
        _data = new zbinary_type[_size];
        memset(_data, dat, _size);
        return *this;
    }

    ZBinary<> &concat(const ZBinary<> &other){
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

    zu64 findFirst(const ZBinary<> &find) const {
        if(find.size() > _size){
            return none;
        }
        zu64 j = 0;
        zu64 start = none;
        for(zu64 i = 0; i < _size; ++i){
            if(_data[i] == find[j]){
                if(j == find.size() - 1)
                    return start;
                if(j == 0)
                    start = i;
                ++j;
            } else {
                if(j){
                    j = 0;
                    i = start + 1;
                }
            }
        }
        return start;
    }

    ZBinary<> getSub(zu64 start, zu64 len) const {
        if(start >= _size)
            return ZBinary<>();
        if(start + len >= _size)
            len = _size - start;
        return ZBinary<>(_data + start, len);
    }

    ZBinary<> &nullTerm(){
        if(_size && _data[_size - 1] != 0){
            resize(_size + 1);
            _data[_size - 1] = 0;
        }
        return *this;
    }

    ZBinary<> printable() const {
        ZBinary<> tmp = *this;
        if(_size){
            tmp.nullTerm();
            for(zu64 i = 0; i < _size - 1; ++i){
                if(_data[i] == 0){
                    _data[i] = '0';
                }
            }
        }
        return tmp;
    }

    const char *asChar() const {
        return (char *)_data;
    }

    zbinary_type &back(){
        return _data[_size - 1];
    }
    const zbinary_type &back() const {
        return _data[_size - 1];
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

    static zu64 hash(HashType type, const ZBinary<> &data){
        switch(type){
        case hashType1: {
            zu64 hash = 5381;
            for(zu64 i = 0; i < data.size(); ++i){
                hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
            }
            return hash;
        }
        default:
            return 0;
        }
    }

private:
    zbinary_type *_data;
    zu64 _size;
    ZStorage *_stor;
};

}

typedef ZBinaryInternal::ZBinary<> ZBinary;

}

#endif // ZBINARY_H
