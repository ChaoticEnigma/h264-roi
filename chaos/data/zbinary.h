/*******************************************************************************
**                                  LibChaos                                  **
**                                  zbinary.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZBINARY_H
#define ZBINARY_H

#include "ztypes.h"
#include "zarray.h"
#include <string.h>
#include "zposition.h"
#include "zreader.h"
#include "zwriter.h"
#include "zerror.h"

namespace LibChaos {

class ZBinary;
typedef ZBinary ZBuffer;

class ZBinary : public ZPosition, public ZReader, public ZWriter {
public:
    typedef zbyte zbinary_type;

    enum {
        none = ZU64_MAX
    };

public:
    ZBinary(ZAllocator<zbyte> *alloc = new ZAllocator<zbyte>) : _alloc(alloc), _data(nullptr), _size(0), _rwpos(0){

    }
    ZBinary(zu64 size) : ZBinary(){
        resize(size);
    }
    ZBinary(const void *ptr, zu64 size) : ZBinary(size){
        if(ptr && size){
            _alloc->rawcopy((const zbyte *)ptr, _data, size);
            _size = size;
        }
    }
    ZBinary(ZArray<zbinary_type> arr) : ZBinary(arr.size()){
        if(arr.size()){
            _alloc->rawcopy(arr.raw(), _data, arr.size());
            _size = arr.size();
        }
    }
    ZBinary(std::initializer_list<zbinary_type> list) : ZBinary(list.size()){
        zu64 i = 0;
        for(auto it = list.begin(); it < list.end(); ++it, ++i){
            _data[i] = *it;
        }
    }
    ZBinary(ZString str) : ZBinary((const zbyte *)str.cc(), str.size()){}

    ZBinary(const ZBinary &other) : ZBinary(other._data, other._size){
        // _rwpos is not copied
    }

    ~ZBinary(){
        clear();
        delete _alloc;
    }
    void clear(){
        _size = 0;
        _realsize = 0;
        _alloc->dealloc(_data);
        _data = nullptr;
    }

    ZBinary &operator=(const ZBinary &other){
        reserve(other._size);
        _size = other._size;
        _alloc->rawcopy(other._data, _data, other._size);
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

    void reserve(zu64 size){
        if(size > _realsize){
            zu64 newsize = 1;
            while(newsize < size) newsize <<= 1;
            zbinary_type *tmp = _alloc->alloc(newsize);
            _alloc->rawcopy(_data, tmp, _size);
            _alloc->dealloc(_data);
            _realsize = newsize;
            _data = tmp;
        }
    }

    void resize(zu64 size){
        if(size < _size || size < _realsize){
            _size = size;
        } else {
            reserve(size);
            _size = size;
        }
    }

    ZBinary &fill(zbinary_type dat, zu64 size){
        reserve(size);
        _size = size;
        memset(_data, dat, _size);
        return *this;
    }

    ZBinary &concat(const ZBinary &other){
        reserve(_size + other._size);
        _alloc->rawcopy(other._data, _data + _size, other._size);
        _size = _size + other._size;
        return *this;
    }

    void reverse(){
        zbinary_type *buffer = _alloc->alloc(_realsize);
        for(zu64 i = 0; i < _size; ++i){
            buffer[i] = _data[_size - i - 1];
        }
        _alloc->dealloc(buffer);
        _data = buffer;
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

    void fromzu8(zu8 num){
        resize(1);
        _data[0] = num;
    }
    void fromzu16(zu16 num){
        resize(2);
        _data[0] = (zu8)((num >> 8) & 0xFF);
        _data[1] = (zu8)((num)      & 0xFF);
    }
    void fromzu32(zu32 num){
        resize(4);
        _data[0] = (zu8)((num >> 24) & 0xFF);
        _data[1] = (zu8)((num >> 16) & 0xFF);
        _data[2] = (zu8)((num >> 8)  & 0xFF);
        _data[3] = (zu8)((num)       & 0xFF);
    }
    void fromzu64(zu64 num){
        resize(8);
        _data[0] = (zu8)((num >> 56) & 0xFF);
        _data[1] = (zu8)((num >> 48) & 0xFF);
        _data[2] = (zu8)((num >> 40) & 0xFF);
        _data[3] = (zu8)((num >> 32) & 0xFF);
        _data[4] = (zu8)((num >> 24) & 0xFF);
        _data[5] = (zu8)((num >> 16) & 0xFF);
        _data[6] = (zu8)((num >> 8)  & 0xFF);
        _data[7] = (zu8)((num)       & 0xFF);
    }

    zu8 tozu8() const {
        zassert(_size == 1);
        return _data[0];
    }
    zu16 tozu16() const {
        zassert(_size == 2);
        return (zu16)(_data[0] << 8) + (zu16)_data[1];
    }
    zu32 tozu32() const {
        zassert(_size == 4);
        return (zu32)(_data[0] << 24) + (zu32)(_data[1] << 16) + (zu32)(_data[2] << 8) + (zu32)_data[3];
    }
    zu64 tozu64() const {
        zassert(_size == 8);
        return ((zu64)_data[0] << 56) + ((zu64)_data[1] << 48) + ((zu64)_data[2] << 40) + ((zu64)_data[3] << 32) +
               ((zu64)_data[4] << 24) + ((zu64)_data[5] << 16) + ((zu64)_data[6] << 8) + (zu64)_data[7];
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
    zu64 realSize() const {
        return _realsize;
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
    ZAllocator<zbinary_type> *_alloc;
    zbinary_type *_data;
    zu64 _size;
    zu64 _realsize;
    zu64 _rwpos;
};

}

#endif // ZBINARY_H
