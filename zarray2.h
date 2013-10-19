#ifndef ZARRAY2_H
#define ZARRAY2_H

#include <cstring>

#include "ztypes.h"

namespace LibChaos {

template <typename T> class ZArrayV2 {
public:
    ZArrayV2() : _size(0), _data(nullptr){}
    ZArrayV2(const ZArrayV2<T> &other) : _size(other.size()), _data(new T[_size * sizeof(T)]){
        //std::memcpy(_data, other.ptr(), _size * sizeof(T));
        memcpy(_data, other.ptr(), _size * sizeof(T));
    }
//    ZArrayV2(T *raw, zu64 len) : _size(len), _data(new T[_size * sizeof(T)]){
//        std::memcpy(_data, raw, len * sizeof(T));
//    }
    ZArrayV2(const T *raw, zu64 len) : _size(len), _data(new T[_size * sizeof(T)]){
        //std::memcpy(_data, raw, len * sizeof(T));
        memcpy(_data, raw, len * sizeof(T));
    }
    ~ZArrayV2(){
        delete[] _data;
    }

    ZArrayV2<T> &assign(ZArrayV2<T> arr){
        _data = arr.ptr();
        return *this;
    }
    inline ZArrayV2<T> &operator=(ZArrayV2<T> arr){
        return assign(arr);
    }

    T &at(zu64 index){
        return _data[index];
    }
    inline T &operator[](zu64 index){
        return at(index);
    }

    ZArrayV2<T> &resize(zu64 len){
        T* tmp = _data;
        _data = new T[len * sizeof(T)];
        std::memcpy(_data, tmp, _size * sizeof(T));
        _size = len;
        delete[] tmp;
        return *this;
    }

    ZArrayV2<T> &push(T value){
        resize(_size + 1);
        back() = value;
        return *this;
    }

    ZArrayV2<T> &erase(zu64 index, zu64 len){
        if(len > 0){
            T* tmp = _data;
            _data = new T[(_size - len) * sizeof(T)];
            if(index > 0)
                std::memcpy(_data, tmp, index);
            std::memcpy(_data + (index * sizeof(T)), tmp + ((index + len) * sizeof(T)), (_size - index - len) * sizeof(T));
            _size = _size - len;
            delete[] tmp;
        }
        return *this;
    }
    inline ZArrayV2<T> &erase(zu64 index){
        return erase(index, 1);
    }

    ZArrayV2<T> &pop(zu64 index){
        return erase(index);
    }
    ZArrayV2<T> &popFront(){
        return pop(0);
    }
    ZArrayV2<T> &popBack(){
        return resize(_size - 1);
    }
    ZArrayV2<T> &popFrontCount(unsigned conut){
        return erase(0, conut);
    }

    ZArrayV2<T> &pushFront(T in){
        T *tmp = _data;
        _data = new T[(_size + 1) * sizeof(T)];
        front() = in;
        concat(ZArrayV2<T>(tmp, _size));
        ++_size;
        return *this;
    }

    ZArrayV2<T> &concat(ZArrayV2<T> in){
        resize(_size + in.size());
        std::memcpy(_data + (_size * sizeof(T)), in.ptr(), in.size() * sizeof(T));
        return *this;
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
    void clear(){
        _size = 0;
        delete[] _data;
        _data = nullptr;
    }

    inline unsigned size() const {
        return _size;
    }
    inline T *ptr() const {
        return _data;
    }

private:
    zu64 _size;
    T *_data;
};

}

#endif // ZARRAY2_H
