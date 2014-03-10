#ifndef ZASSOC_H
#define ZASSOC_H

#include "zarray.h"

#if ZASSOC_VERSION == 1

namespace LibChaos {

template <class K, class T> class ZAssoc {
public:
    struct Data {
        K key;
        T val;
    };

    ZAssoc(){}
    ZAssoc(ZArray<T> in){
        for(unsigned i = 0; i < in.size(); ++i){
            push(in[i]);
        }
    }
    ZAssoc(std::initializer_list<Data> ls){
        for(auto i = ls.begin(); i < ls.end(); ++i){
            push(i->key, i->val);
        }
    }

    ~ZAssoc(){}

    T &at(zu64 index){
        return _data[index].val;
    }
    T &operator[](zu64 index){
        return at(index);
    }

    T &pos(zu64 index){
        return _data[index].val;
    }

    T &at(K key_){
        for(zu64 i = 0; i < _data.size(); ++i){
            if(_data[i].key == key_)
                return _data[i].val;
        }
        _data.push({ key_, T() });
        //T temp = {};
        //data.push({ key_, temp });
        return last();
    }
    T &operator[](K key_){
        return at(key_);
    }
    const T &operator[](K key_) const {
        for(zu64 i = 0; i < _data.size(); ++i){
            if(_data[i].key == key_)
                return _data[i].val;
        }
        throw "Invalid access to const ZAssoc";
    }

    K &key(zu64 index){
        if(index < size()){
            return _data[index].key;
        } else {
            K temp[1] = {};
            _empk = temp[0];
            //empk = K();
            return _empk;
        }
    }

    ZAssoc<K, T> &push(K key_, T value){
        _data.push({ key_, value });
        //data.push({ key_, value });
        return *this;
    }
    ZAssoc<K, T> &push(T value){
        return push(K(), value);
    }
    ZAssoc<K, T> &pushFront(K key_, T value){
        _data.pushFront({ key_, value });
        //data.pushFront({ key, value });
        return *this;
    }
    ZAssoc<K, T> &pushFront(T value){
        return pushFront(K(), value);
    }

    ZAssoc<K, T> &pop(unsigned index){
        _data.pop(index);
        return *this;
    }
    ZAssoc<K, T> &popAll(K key_){
        for(unsigned i = 0; i < _data.size(); ++i){
            if(_data[i].key == key_){
                pop(i);
                i = 0;
            }
        }
        return *this;
    }
    ZAssoc<K, T> &popFront(){
        _data.popFront();
        return *this;
    }
    ZAssoc<K, T> &popBack(){
        _data.popBack();
        return *this;
    }
    ZAssoc<K, T> &popFrontCount(unsigned cnt){
        _data.popFrontCount(cnt);
        return *this;
    }

    ZAssoc<K, T> &concat(ZAssoc<K, T> in){
        _data.concat(in.dat());
        return *this;
    }

    ZAssoc<K, T> &insert(zu64 pos, K key, T val){
        _data.insert(pos, { key, val });
        return *this;
    }

    bool exists(K test) const {
        for(zu64 i = 0; i < size(); ++i){
            if(_data[i].key == test)
                return true;
        }
        return false;
    }
    bool contains(T test) const {
        for(zu64 i = 0; i < size(); ++i){
            if(_data[i].val == test)
                return true;
        }
        return false;
    }

    K find(T test){
        for(zu64 i = 0; i < size(); ++i){
            if(_data[i].val == test)
                return _data[i].key;
        }
        K temp[1] = {};
        _empk = temp[0];
        return _empk;
    }

    T &first(){
        return _data.front().val;
    }
    T &last(){
        return _data.back().val;
    }
    T &firstKey(){
        return _data.front().key;
    }
    T &lastKey(){
        return _data.back().key;
    }

    bool empty() const {
        return _data.empty();
    }

    unsigned size() const {
        return _data.size();
    }
    ZArray<Data> &dat(){
        return _data;
    }

private:
    ZArray<Data> _data;
    K _empk;
};

}

#else

#include "zassoc2.h"

#endif

#endif // ZASSOC_H
