#ifndef ZSTORAGE_H
#define ZSTORAGE_H

#include "ztypes.h"

namespace LibChaos {

class ZStorage {
public:
    virtual ~ZStorage(){}
    virtual void copy(const zbyte *src, zu64 size) = 0;
    virtual zbyte &at(zu64 index) = 0;
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

    zbyte &at(zu64 index){
        return _data[index];
    }

    void resize(zu64 size){
        if(size == 0){
            clear();
            return;
        } else if(size >= _realsize){
            if(_size == 0)
                _realsize = size;
            else
                _realsize = _size * 2;
            zbyte *tmp = _data;
            _data = new zbyte[_realsize];
            copy(tmp, _size);
        }
        _size = size;
    }

    void clear(){
        delete[] _data;
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
