/*******************************************************************************
**                                  LibChaos                                  **
**                                  zbinary.h                                 **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#ifndef ZBINARY_H
#define ZBINARY_H

#include "ztypes.h"
#include "zarray.h"
#include <string.h>
#include "zposition.h"
#include "zreader.h"
#include "zwriter.h"

namespace LibChaos {

class ZBinary;
typedef ZBinary ZBuffer;

class ZBinary : public ZPosition, public ZReader, public ZWriter {
public:
    typedef unsigned char zbinary_type;

    enum {
        none = ZU64_MAX
    };

public:
    ZBinary() : _data(nullptr), _size(0), _rwpos(0){

    }
    ZBinary(zu64 size) : ZBinary(){
        _size = size;
        _data = new zbinary_type[_size];
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
    ZBinary(const ZBinary &other) : ZBinary(other._data, other._size){
        // _rwpos is not copied
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
        return (memcmp(_data, rhs._data, _size) == 0);
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

    zu64 findFirst(const ZBinary &find) const {
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

    ZBinary getSub(zu64 start, zu64 len) const {
        if(start >= _size)
            return ZBinary();
        if(start + len >= _size)
            len = _size - start;
        return ZBinary(_data + start, len);
    }

    ZBinary &nullTerm(){
        if(_size && _data[_size - 1] != 0){
            resize(_size + 1);
            _data[_size - 1] = 0;
        }
        return *this;
    }

    ZBinary printable() const {
        ZBinary tmp = *this;
        if(_size){
            tmp.nullTerm();
            for(zu64 i = 0; i < _size - 1; ++i){
                if(_data[i] == 0){
                    _data[i] = '0';
                } else if(_data[i] > 127){
                    _data[i] = '!';
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

    zbinary_type *raw(){
        return _data;
    }
    const zbinary_type *raw() const {
        return _data;
    }

    zu64 size() const {
        return _size;
    }

    // ZReader interface
    zu64 read(zbyte *dest, zu64 length){
        if(_rwpos + length > size())
            length = _rwpos + length - size();
        if(dest && length)
            memcpy(dest, _data + _rwpos, length);
        _rwpos += length;
        return length;
    }
    zu64 rewind(){
        return setPos(0);
    }
    bool atEnd() const {
        return _rwpos == size();
    }
    zu64 setPos(zu64 pos){
        _rwpos = pos;
        return _rwpos;
    }
    zu64 getPos() const {
        return _rwpos;
    }

    // ZWrite interface
    zu64 write(const zbyte *data, zu64 size){
        if(size > _size - _rwpos)
            resize(_rwpos + size);
        memcpy(_data + _rwpos, data, size);
        _rwpos += size;
        return size;
    }

private:
    zbinary_type *_data;
    zu64 _size;
    zu64 _rwpos;
};

}

#endif // ZBINARY_H
