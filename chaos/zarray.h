#ifndef ZARRAY_H
#define ZARRAY_H

#include "ztypes.h"

#include <vector>

#if ZARRAY_VERSION == 1

namespace LibChaos {

template <typename T> class ZArray {
public:
    ZArray(){}
    ZArray(T first){
        push(first);
    }
    ZArray(const T *raw, zu64 len) : _data(raw, raw + len){}
    ~ZArray(){}

//    ZArray<T> &operator=(ZArray<T> arr){
//        data = arr.dat();
//        return *this;
//    }

    bool operator==(ZArray<T> arr){
        return (_data == arr.data());
    }
    bool operator!=(ZArray<T> arr){
        return operator==(arr);
    }

    T &at(zu64 index){
        if(index < size()){
            return _data[index];
        } else {
            //T tmp = {};
            //T temp[1] = {};
            //empv = temp[0];
            return empv;
        }
    }
    T &operator[](zu64 index){
        return at(index);
    }

    ZArray<T> &resize(zu64 len){
        _data.resize(len);
        return *this;
    }

    ZArray<T> &push(T value){
        _data.push_back(value);
        return *this;
    }
    inline ZArray<T> &operator<<(T value){
        return push(value);
    }

    ZArray<T> &erase(zu64 index, zu64 count){
        _data.erase(_data.begin() + index, _data.begin() + index + count);
        return *this;
    }
    ZArray<T> &erase(zu64 index){
        return erase(index, 1);
    }

    ZArray<T> &pop(zu64 index){
        return erase(index);
    }
    ZArray<T> &popFront(){
        return pop(0);
    }
    ZArray<T> &popBack(){
        if(!empty()){
            _data.pop_back();
        }
        return *this;
    }
    ZArray<T> &popFrontCount(unsigned conut){
        return erase(0, conut);
    }

    ZArray<T> &pushFront(T in){
        typename std::vector<T>::iterator it;
        it = _data.begin();
        _data.insert(it, in);
        return *this;
    }

    ZArray<T> &concat(ZArray<T> in){
        for(unsigned i = 0; i < in.size(); ++i){
            _data.push_back(in[i]);
        }
        return *this;
    }

//    ZArrayV1<T> &clean(){
//        for(long int i = 0; i < size(); ++i){
//            if(data[i] == T()){
//                pop(i);
//                i = -1;
//            }
//        }
//        return *this;
//    }

//    bool contains(T item){
//        for(unsigned i = 0; i < data.size(); ++i)
//            if(data[i] == item)
//                return true;
//        return false;
//    }

    T &front(){
        return _data.front();
    }
    T &back(){
        return _data.back();
    }

    bool contains(T test){
        for(unsigned i = 0; i < size(); ++i){
            if(_data[i] == test)
                return true;
        }
        return false;
    }

    bool empty() const {
        if(_data.size() <= 0)
            return true;
        return false;
    }
    void clear(){
        _data.clear();
    }

    std::vector<T> &data(){
        return _data;
    }
    zu64 size() const {
        return _data.size();
    }
    const T *ptr() const {
        return _data.data();
    }

private:
    std::vector<T> _data;
    T empv;
};

}

#else

#include "zarray2.h"

#endif

#endif // ZARRAY_H
