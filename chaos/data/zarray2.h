#ifndef ZARRAY2_H
#define ZARRAY2_H

#include <cstring>

#include "ztypes.h"

namespace LibChaos {

template <typename T> class ZArray {
public:
    ZArray() : _size(0), _data(nullptr){

    }
    ZArray(const T *raw, zu64 size) : _size(size), _data(nullptr){
        if(_size && raw != nullptr){
            _data = new T[_size];
            for(zu64 i = 0; i < _size; ++i){
                _data[i] = raw[i];
            }
            //memcpy(_data, raw, _size * sizeof(T));
        }
    }
    ZArray(T first) : ZArray(&first, 1){

    }
    ZArray(const ZArray<T> &other) : ZArray(other._data, other._size){

    }

    ~ZArray(){
        clear();
    }

    void clear(){
        _size = 0;
        delete[] _data;
        _data = nullptr;
    }

    ZArray<T> &operator=(const ZArray<T> &other){
        if(other.size() && other.ptr() != nullptr){
            clear();
            _size = other.size();
            _data = new T[_size];
            for(zu64 i = 0; i < _size; ++i){
                _data[i] = other._data[i];
            }
            //memcpy(_data, other.ptr(), _size * sizeof(T));
        } else {
            clear();
        }
        return *this;
    }

    bool operator==(const ZArray<T> &arr) const {
        if(size() != arr.size())
            return false;
        for(zu64 i = 0; i < size(); ++i){
            if(!(operator[](i) == arr[i]))
                return false;
        }
        return true;
    }
    bool operator!=(const ZArray<T> &arr) const {
        return !operator==(arr);
    }

    T &operator[](zu64 index){
        return _data[index];
    }
    T &at(zu64 index){
        return operator[](index);
    }

    const T &operator[](zu64 index) const {
        return _data[index];
    }
    const T &at(zu64 index) const {
        return operator[](index);
    }

    ZArray<T> &resize(zu64 size){
        if(size){
            T *tmp = new T[size];
            if(_size && tmp != nullptr && _data != nullptr)
                memcpy(tmp, _data, MIN(size, _size) * sizeof(T));
            operator delete[] _data;
            _data = tmp;
            _size = size;
        } else {
            clear();
        }
        return *this;
    }

    ZArray<T> &extend(zu64 num = 1){
        return resize(_size + num);
    }

    ZArray<T> &push(T value){
        resize(_size + 1);
        back() = value;
        return *this;
    }

    ZArray<T> &pushFront(const T &in){
        T *tmp = new T[_size + 1];
        tmp[0] = in;
        if(_size && _data != nullptr)
            memcpy(tmp + 1, _data, _size * sizeof(T));
        delete[] _data;
        _data = tmp;
        ++_size;
        return *this;
    }

    ZArray<T> &insert(zu64 pos, const T &in){
        T *tmp = new T[_size + 1];
        tmp[0] = in;
        memcpy(tmp, _data, pos * sizeof(T));
        _data[pos] = in;
        memcpy(tmp + pos + 1, _data + pos, (_size - pos) * sizeof(T));
        delete[] _data;
        _data = tmp;
        ++_size;
        return *this;
    }

    ZArray<T> &concat(const ZArray<T> &in){
        zu64 presize = _size;
        resize(_size + in.size());
        for(zu64 i = 0; i < in.size(); ++i){
            _data[presize + i] = in.ptr()[i];
        }
        //memcpy(_data + (_size * sizeof(T)), in.ptr(), in.size() * sizeof(T));
        return *this;
    }

    ZArray<T> &erase(zu64 index, zu64 count){
        if(_size && count){
            if(count < _size && (index || (_size - index - count))){
                T *tmp = _data;
                _data = new T[_size - count];
                if(index)
                    memcpy(_data, tmp, index * sizeof(T));
                if(_size - index - count)
                    memcpy(_data + (index * sizeof(T)), tmp + ((index + count) * sizeof(T)), (_size - index - count) * sizeof(T));
                delete[] tmp;
                _size = _size - count;
            } else {
                clear();
            }
        }
        return *this;
    }
    inline ZArray<T> &erase(zu64 index){
        return erase(index, 1);
    }

    ZArray<T> &pop(zu64 index){
        return erase(index);
    }
    ZArray<T> &popFront(){
        return pop(0);
    }
    ZArray<T> &popBack(){
        return resize(_size - 1);
    }
    ZArray<T> &popFrontCount(unsigned conut){
        return erase(0, conut);
    }

    bool contains(const T &test) const {
        for(zu64 i = 0; i < _size; ++i){
            if(_data[i] == test)
                return true;
        }
        return false;
    }

    T &front(){
        return _data[0];
    }
    T &back(){
        return _data[_size - 1];
    }

    bool empty() const {
        return (_size == 0);
    }

    inline zu64 size() const {
        return _size;
    }
    inline T *ptr() const {
        return _data;
    }
    inline T *raw() const {
        return ptr();
    }

private:
    zu64 _size;
    T *_data;
};

}

#endif // ZARRAY2_H
