/*****************************************
**               LibChaos               **
**               zarray.h               **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZARRAY_H
#define ZARRAY_H

#include "ztypes.h"

#include <vector>
#include <initializer_list>

#if ZARRAY_VERSION == 1

namespace LibChaos {

template <typename T> class ZArray {
public:
    enum {
        none = (zu64)-1
    };

public:
    ZArray(){}
    ZArray(T first){
        push(first);
    }
    ZArray(const T *raw, zu64 len) : _data(raw, raw + len){}
    ZArray(std::initializer_list<T> ls){
        for(auto i = ls.begin(); i < ls.end(); ++i){
            push(*i);
        }
    }

    ~ZArray(){}

//    ZArray<T> &operator=(ZArray<T> arr){
//        data = arr.dat();
//        return *this;
//    }

    bool operator==(ZArray<T> arr) const {
        return (_data == arr.data());
    }
    bool operator!=(ZArray<T> arr) const {
        return !operator==(arr);
    }

    T &at(zu64 index){
        if(index < size()){
            return _data[index];
        }
        throw "Invalid ZArray index";
    }
    T &operator[](zu64 index){
        return at(index);
    }
    const T &at(zu64 index) const {
        if(index < size()){
            return _data[index];
        }
        throw "Invalid ZArray index";
    }
    const T &operator[](zu64 index) const {
        return at(index);
    }
    // Const-only overload
    const T &get(zu64 index) const {
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

    ZArray<T> &extend(zu64 num = 1){
        return resize(size() + num);
    }

    ZArray<T> &pop(zu64 index){
        return erase(index);
    }
    ZArray<T> &popFront(){
        return pop(0);
    }
    ZArray<T> &popBack(){
        if(!isEmpty()){
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

    ZArray<T> &insert(zu64 pos, T in){
        typename std::vector<T>::iterator it = _data.begin() + pos;
        _data.insert(it, in);
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

    zu64 indexOf(T test) const {
        for(zu64 i = 0; i < size(); ++i){
            if(_data[i] == test)
                return i;
        }
        return none;
    }

    bool contains(T test){
        for(unsigned i = 0; i < size(); ++i){
            if(_data[i] == test)
                return true;
        }
        return false;
    }

    void swap(ZArray<T>& other){
        _data.swap(other._data);
    }

    bool isEmpty() const {
        return _data.size() == 0;
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
};

}

#else

#include "zarray2.h"

#endif

#endif // ZARRAY_H
