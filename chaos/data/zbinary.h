/*******************************************************************************
**                                  LibChaos                                  **
**                                  zbinary.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZBINARY_H
#define ZBINARY_H

#include "ztypes.h"
#include "zarray.h"
#include "zposition.h"
#include "zreader.h"
#include "zwriter.h"
#include "zerror.h"
#include "zhash.h"

namespace LibChaos {

class ZBinary;
typedef ZBinary ZBuffer;

//! Binary data buffer container.
class ZBinary : public ZPosition, public ZReader, public ZWriter {
public:
    typedef zbyte zbinary_type;

    enum {
        none = ZU64_MAX
    };

public:
    ZBinary(ZAllocator<zbyte> *alloc = new ZAllocator<zbyte>) : _alloc(alloc), _data(nullptr), _size(0), _realsize(0), _rwpos(0){

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

    //! Change the logical size of the buffer.
    //! Allocate new memory only if necessary.
    void resize(zu64 size){
        reserve(size);
        _size = size;
    }

    ZBinary &fill(zbinary_type dat, zu64 size);

    ZBinary &concat(const ZBinary &other);

    ZBinary &append(zbinary_type byte);

    void reverse();

    zu64 findFirst(const ZBinary &find) const;

    ZBinary getSub(zu64 start, zu64 len) const;

    static ZBinary fromzu8(zu8 num);
    static ZBinary fromzu16(zu16 num);
    static ZBinary fromzu32(zu32 num);
    static ZBinary fromzu64(zu64 num);

    zu8 tozu8() const;
    zu16 tozu16() const;
    zu32 tozu32() const;
    zu64 tozu64() const;

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
    zu64 read(ZBinary &dest, zu64 length){ return read(dest.raw(), length); }

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
    zu64 write(const ZBinary &data){ return write(data.raw(), data.size()); }

    zu8 readzu8();
    zu16 readzu16();
    zu32 readzu32();
    zu64 readzu64();

    void writezu8(zu8 num);
    void writezu16(zu16 num);
    void writezu32(zu32 num);
    void writezu64(zu64 num);

    zu16 readle16();
    zu32 readle32();
    zu64 readle64();

    void writele16(zu16 num);
    void writele32(zu32 num);
    void writele64(zu64 num);

    //! Decode 1 byte into unsigned 8-bit integer
    static zu8 deczu8(const zbyte *bin);
    //! Encode unsigned 8-bit integer into 1 byte
    static void enczu8(zbyte *bin, zu8 num);

    //! Aliases for decoding multi-byte integers from bytes
    static zu16 deczu16(const zbyte *bin){ return decbe16(bin); }
    static zu32 deczu32(const zbyte *bin){ return decbe32(bin); }
    static zu64 deczu64(const zbyte *bin){ return decbe64(bin); }

    //! Aliases for encoding multi-byte integers into bytes
    static void enczu16(zbyte *bin, zu16 num){ encbe16(bin, num); }
    static void enczu32(zbyte *bin, zu32 num){ encbe32(bin, num); }
    static void enczu64(zbyte *bin, zu64 num){ encbe64(bin, num); }

    //! Decode 2 big-endian bytes into unsigned 16-bit integer
    static zu16 decbe16(const zbyte *bin);
    //! Decode 4 big-endian bytes into unsigned 32-bit integer
    static zu32 decbe32(const zbyte *bin);
    //! Decode 8 big-endian bytes into unsigned 64-bit integer
    static zu64 decbe64(const zbyte *bin);

    //! Encode unsigned 16-bit integer into 2 big-endian bytes
    static void encbe16(zbyte *bin, zu16 num);
    //! Encode unsigned 32-bit integer into 4 big-endian bytes
    static void encbe32(zbyte *bin, zu32 num);
    //! Encode unsigned 64-bit integer into 8 big-endian bytes
    static void encbe64(zbyte *bin, zu64 num);

    //! Decode 2 little-endian bytes into unsigned 16-bit integer
    static zu16 decle16(const zbyte *bin);
    //! Decode 4 little-endian bytes into unsigned 32-bit integer
    static zu32 decle32(const zbyte *bin);
    //! Decode 8 little-endian bytes into unsigned 64-bit integer
    static zu64 decle64(const zbyte *bin);

    //! Encode unsigned 16-bit integer into 2 little-endian bytes
    static void encle16(zbyte *bin, zu16 num);
    //! Encode unsigned 32-bit integer into 4 little-endian bytes
    static void encle32(zbyte *bin, zu32 num);
    //! Encode unsigned 64-bit integer into 8 little-endian bytes
    static void encle64(zbyte *bin, zu64 num);

    //! Decode 4 big-endian bytes into a 32-bit double
    static float decfloat(const zbyte *bin);
    //! Decode 8 big-endian bytes into a 64-bit double
    static double decdouble(const zbyte *bin);

    //! Encode a 32-bit double into 4 big-endian bytes
    static void encfloat(zbyte *bin, float flt);
    //! Encode a 64-bit double into 8 big-endian bytes
    static void encdouble(zbyte *bin, double dbl);

private:
    ZAllocator<zbinary_type> *_alloc;
    zbinary_type *_data;
    zu64 _size;
    zu64 _realsize;
    zu64 _rwpos;
};

// ZBinary specialization ZHash
ZHASH_USER_SPECIALIAZATION(ZBinary, (const ZBinary &bin), (bin.raw(), bin.size()), {})

}

#endif // ZBINARY_H
