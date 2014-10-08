#ifndef ZDEFAULTSTORAGE_H
#define ZDEFAULTSTORAGE_H

#include "zstorage.h"
#include <cstring>

namespace LibChaos {

class ZMemoryStorage : public ZStorage {
public:
    static const zu16 type = 1;

public:
    ZMemoryStorage() : _size(0), _realsize(0), _data(nullptr){

    }
    ZMemoryStorage(zu64 size) : ZMemoryStorage(){
        resize(size);
    }
    ZMemoryStorage(const ZMemoryStorage &other) : ZMemoryStorage(){
        copy(&other);
    }

    ~ZMemoryStorage(){
        delete[] _data;
    }

    void copy(const ZStorage *other){
        if(other != nullptr){
            resize(other->size());
            other->copyBlockTo(0, other->size(), _data);
        }
    }

    void copyToBlock(const zbyte *data, zu64 index, zu64 size){
        resize(index + size);
        if(size != 0 && data != nullptr){
            _copy(data, _data + index, size);
        }
    }

    void copyBlockTo(zu64 index, zu64 size, zbyte *data) const {
        if(size != 0 && data != nullptr && index < _size){
            _copy(_data + index, data, MIN(size, _size - index));
        }
    }

    ZStorage *newCopy() const {
        ZMemoryStorage *tmp = new ZMemoryStorage;
        tmp->copy(this);
        return tmp;
    }

    inline zbyte get(zu64 index) const {
        return _data[index];
    }
    inline void set(zu64 index, zbyte byte){
        _data[index] = byte;
    }

    template <typename T>
    inline T getType(zu64 index) const {
        return ((T*)_data)[index];
    }
    template <typename T>
    inline void setType(zu64 index, T data){
        ((T*)_data)[index] = data;
    }

    zbyte *getBlock(zu64 index, zu64 size){
        return _data + index;
    }

    void freeBlock(zbyte *){
        // The default storage type doesn't need to do anything
    }
    void commitBlock(zbyte *){
        // The default storage type doesn't need to do anything
    }

    virtual bool compare(const ZStorage *other) const {
        if(_size != other->size())
            return false;
        if(other->storageType() == type){
            return memcmp(_data, ((const ZMemoryStorage *)other)->_data, _size) == 0;
        } else {
            for(zu64 i = 0; i < _size; ++i){
                if(get(i) != other->get(i))
                    return false;
            }
            return true;
        }
    }

    void resize(zu64 newsize){
        if(newsize == 0){
            // If new size is zero, clear
            clear();
        } else if(newsize <= _realsize){
            // If new size is less than reserved size
            _size = newsize;
        } else {
            // If new size is larger
            if(_realsize == 0){
                // Old size is 0
                delete[] _data; // Just in case
                _realsize = newsize;
                _size = newsize;
                _data = new zbyte[_realsize];
            } else {
                // Old data needs to be copied
                if(newsize > (_realsize << 1)){
                    // If new size is a lot larger than the reserved size
                    _realsize = newsize;
                } else {
                    // Otherwise new size is less than twice as large as the reserved size
                    _realsize <<= 1;
                }
                // Copy to new memory
                zbyte *tmp = _data;
                _data = new zbyte[_realsize];
                _copy(tmp, _data, _size);
                delete[] tmp;
                _size = newsize;
            }
        }
    }

    void clear(){
        delete[] _data;
        _data = nullptr;
        _size = 0;
        _realsize = 0;
    }

    inline zu64 size() const {
        return _size;
    }

    inline zu64 realSize() const {
        return _realsize;
    }

    inline zu16 storageType() const {
        return type;
    }

    inline const char *storageTypeStr() const {
        return "ZMemoryStorage";
    }

private:
    static void _copy(const zbyte *src, zbyte *dest, zu64 size){
        if(size != 0 && dest != nullptr && src != nullptr)
            memcpy(dest, src, size * sizeof(zbyte));
    }

private:
    zu64 _size;
    zu64 _realsize;
    zbyte *_data;
};

typedef ZMemoryStorage ZDefaultStorage;

}

#endif // ZDEFAULTSTORAGE_H
