#ifndef ZSTORAGE_H
#define ZSTORAGE_H

#include "ztypes.h"

namespace LibChaos {

class ZStorage {
public:
    virtual ~ZStorage(){}
    virtual zbyte &get(zu64 index) = 0;
    virtual void resize(zu64 size) = 0;
    virtual void clear() = 0;
    virtual zu64 size() const = 0;
};

class ZDefaultStorage : public ZStorage {
public:
    ZDefaultStorage() : _size(0), _realsize(0), _data(nullptr){

    }
    ZDefaultStorage(zu64 size) : ZDefaultStorage() {
        resize(size);
    }
    ZDefaultStorage(const ZDefaultStorage &other) : ZDefaultStorage() {
        resize(other._size);
        copy(other._data, other._size);
    }

    ~ZDefaultStorage(){
        clear();
    }

    void copy(const zbyte *src, zu64 size){
        if(size != 0 && _data != nullptr && src != nullptr)
            memcpy(_data, src, size * sizeof(zbyte));
    }

    zbyte &get(zu64 index){
        return _data[index];
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
                copy(tmp, _size);
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

    zu64 size() const {
        return _size;
    }

    zu64 realSize() const {
        return _realsize;
    }

private:
    zu64 _size;
    zu64 _realsize;
    zbyte *_data;
};

}

#endif // ZSTORAGE_H
