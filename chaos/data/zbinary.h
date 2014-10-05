#ifndef ZBINARY_H
#define ZBINARY_H

#include "ztypes.h"
#include "zarray.h"
#include <string.h>

#include "zreader.h"
#include "zwriter.h"

#include "zstorage.h"
#include "zdefaultstorage.h"

namespace LibChaos {

class ZBinary : public ZReader, public ZWriter {
public:

    typedef zbyte zbinary_type;

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
    ZBinary() : _stor(new ZDefaultStorage), _rwpos(0){

    }
    ZBinary(ZStorage *stor) : _stor(stor->newCopy()), _rwpos(0){

    }
    ZBinary(const ZBinary &other) : _stor(other.storage()->newCopy()), _rwpos(0){

    }

    ZBinary(const void *ptr, zu64 size) : ZBinary(){
        _stor->copyToBlock((const zbyte *)ptr, 0, size);
    }
    ZBinary(ZArray<zbinary_type> arr) : ZBinary(){
        _stor->copyToBlock((const zbyte *)arr.ptr(), 0, arr.size());
    }
    ZBinary(std::initializer_list<zbinary_type> list) : ZBinary(){
        _stor->resize(list.size());
        zu64 i = 0;
        for(auto it = list.begin(); it < list.end(); ++it, ++i){
            _stor->get(i) = *it;
        }
    }

    ~ZBinary(){
        delete _stor;
    }
    void clear(){
        _stor->clear();
    }

    ZBinary &operator=(const ZBinary &other){
        _stor->copy(other.storage());
        return *this;
    }

    bool operator==(const ZBinary &rhs) const {
        return _stor->compare(rhs.storage());
    }
    bool operator!=(const ZBinary &rhs) const {
        return !operator==(rhs);
    }

    zbinary_type &operator[](zu64 index){
        return _stor->get(index);
    }
    const zbinary_type &operator[](zu64 index) const {
        return _stor->get(index);
    }

    ZBinary &resize(zu64 size){
        _stor->resize(size);
        return *this;
    }

    ZBinary &fill(zbinary_type dat, zu64 size){
        resize(size);
        for(zu64 i = 0; i < size; ++i){
            _stor->get(i) = dat;
        }
        return *this;
    }

    ZBinary &concat(const ZBinary &other){
        zu64 old = size();
        resize(size() + other.size());
        zbyte *dest = _stor->getBlock(old, other.size());
        zbyte *src = other.storage()->getBlock(0, other.size());
        memcpy(dest, src, other.size());
        _stor->commitBlock(dest);
        other.storage()->freeBlock(src);
        return *this;
    }

    void reverse(){
        ZStorage *buff = _stor->newCopy();
        for(zu64 i = 0, j = size(); i < buff->size(); ++i, --j){
            _stor->get(j) = buff->get(i);
        }
        delete buff;
    }

    zu64 findFirst(const ZBinary &find) const {
        if(find.size() > size()){
            return none;
        }
        zu64 j = 0;
        zu64 start = none;
        for(zu64 i = 0; i < size(); ++i){
            if(_stor->get(i) == find[j]){
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
        if(start >= size())
            return ZBinary();
        if(start + len >= size())
            len = size() - start;
        ZBinary tmp;
        tmp.resize(len);
        zbyte *src = _stor->getBlock(start, len);
        tmp.storage()->copyToBlock(src, 0, len);
        _stor->freeBlock(src);
        return tmp;
    }

    ZBinary &nullTerm(){
        if(size() && _stor->get(size() - 1) != 0){
            resize(size() + 1);
            _stor->get(size() - 1) = 0;
        }
        return *this;
    }

    ZBinary printable() const {
        ZBinary tmp = *this;
        if(size()){
            tmp.nullTerm();
            for(zu64 i = 0; i < size() - 1; ++i){
                if(_stor->get(i) == 0){
                    _stor->get(i) = '0';
                }
            }
        }
        return tmp;
    }

    inline zbinary_type &back(){
        return _stor->get(size() - 1);
    }
    inline const zbinary_type &back() const {
        return _stor->get(size() - 1);
    }

    inline zu64 size() const {
        return _stor->size();
    }

    ZStorage *storage() const {
        return _stor;
    }

    static zu64 hash(HashType type, const ZBinary &data){
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

    // ZReader interface
    zu64 read(unsigned char *dest, zu64 length){
        if(_rwpos + length > size()){
            length = _rwpos + length - size();
        }
        if(dest && length)
            _stor->copyBlockTo(_rwpos, length, dest);
        _rwpos += length;
        return length;
    }
    void rewind(){
        _rwpos = 0;
    }
    bool atEnd() const {
        return _rwpos == size();
    }
    void setReadPos(zu64 pos){
        _rwpos = pos;
    }
    zu64 readPos() const {
        return _rwpos;
    }

    // ZWrite interface
    zu64 write(const zbyte *data, zu64 size){
        _stor->copyToBlock(data, _rwpos, size);
        _rwpos += size;
        return size;
    }

private:
    ZStorage *_stor;
    zu64 _rwpos;
};

}

#endif // ZBINARY_H
