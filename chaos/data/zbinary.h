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
        if(size > _realsize)
            reserve(size);
        _size = size;
    }

    ZBinary &fill(zbinary_type dat, zu64 size){
        reserve(size);
        _size = size;
        memset(_data, dat, _size);
        return *this;
    }

    ZBinary &concat(const ZBinary &other);

    void reverse();

    zu64 findFirst(const ZBinary &find) const;

    ZBinary getSub(zu64 start, zu64 len) const;

    void fromzu8(zu8 num){
        resize(1);
        enczu8(_data, num);
    }
    void fromzu16(zu16 num){
        resize(2);
        enczu16(_data, num);
    }
    void fromzu32(zu32 num){
        resize(4);
        enczu32(_data, num);
    }
    void fromzu64(zu64 num){
        resize(8);
        enczu64(_data, num);
    }

    zu8 tozu8() const {
        zassert(_size == 1);
        return deczu8(_data);
    }
    zu16 tozu16() const {
        zassert(_size == 2);
        return deczu16(_data);
    }
    zu32 tozu32() const {
        zassert(_size == 4);
        return deczu32(_data);
    }
    zu64 tozu64() const {
        zassert(_size == 8);
        return deczu64(_data);
    }

    ZBinary &nullTerm();
    ZBinary printable() const;

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
    zu64 read(zbyte *dest, zu64 length);

    ZBinary readSub(zu64 length){
        ZBinary bin(length);
        return read(bin.raw(), length);
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
    zu64 write(const zbyte *data, zu64 size);

    zu8 readzu8();
    zu16 readzu16();
    zu32 readzu32();
    zu64 readzu64();

    void writezu8(zu8 num);
    void writezu16(zu16 num);
    void writezu32(zu32 num);
    void writezu64(zu64 num);

    //! Encode unsigned 8-bit integer into 1 byte
    static void enczu8(zbyte *bin, zu8 num);
    //! Encode unsigned 16-bit integer into 2 bytes
    static void enczu16(zbyte *bin, zu16 num);
    //! Encode unsigned 32-bit integer into 4 bytes
    static void enczu32(zbyte *bin, zu32 num);
    //! Encode unsigned 64-bit integer into 8 bytes
    static void enczu64(zbyte *bin, zu64 num);

    //! Decode 1 byte into unsigned 8-bit integer
    static zu8 deczu8(const zbyte *bin);
    //! Decode 2 bytes into unsigned 16-bit integer
    static zu16 deczu16(const zbyte *bin);
    //! Decode 4 bytes into unsigned 32-bit integer
    static zu32 deczu32(const zbyte *bin);
    //! Decode 8 bytes into unsigned 64-bit integer
    static zu64 deczu64(const zbyte *bin);

private:
    ZAllocator<zbinary_type> *_alloc;
    zbinary_type *_data;
    zu64 _size;
    zu64 _realsize;
    zu64 _rwpos;
};

}

#endif // ZBINARY_H
