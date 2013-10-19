#ifndef ZASSOC_H
#define ZASSOC_H

#include "zarray.h"

namespace LibChaos {

template <class K, class T> class ZAssoc {
public:
    struct Data {
        Data(){}
        Data(K k, T v) : key(k), val(v){}
        K key;
        T val;
    };

    ZAssoc(){}
    ZAssoc(ZArray<T> in){
        for(unsigned i = 0; i < in.size(); ++i){
            push(in[i]);
        }
    }
    ~ZAssoc(){}

    T &at(unsigned index){
        return data[index].val;
    }
    T &operator[](unsigned index){
        return at(index);
    }

    T &at(K key_){
        for(unsigned i = 0; i < data.size(); ++i){
            if(data[i].key == key_)
                return data[i].val;
        }
        data.push(Data(key_, T()));
        //T temp = {};
        //data.push({ key_, temp });
        return last();
    }
    T &operator[](K key_){
        return at(key_);
    }

    K &key(unsigned index){
        if(index < size()){
            return data[index].key;
        } else {
            K temp[1] = {};
            empk = temp[0];
            //empk = K();
            return empk;
        }
    }

    ZAssoc<K, T> &push(K key_, T value){
        data.push(Data(key_, value));
        //data.push({ key_, value });
        return *this;
    }
    ZAssoc<K, T> &push(T value){
        return push(K(), value);
    }
    ZAssoc<K, T> &pushFront(K key_, T value){
        data.pushFront(Data(key_, value));
        //data.pushFront({ key, value });
        return *this;
    }
    ZAssoc<K, T> &pushFront(T value){
        return pushFront(K(), value);
    }

    ZAssoc<K, T> &pop(unsigned index){
        data.pop(index);
        return *this;
    }
    ZAssoc<K, T> &popAll(K key_){
        for(unsigned i = 0; i < data.size(); ++i){
            if(data[i].key == key_){
                pop(i);
                i = 0;
            }
        }
        return *this;
    }
    ZAssoc<K, T> &popFront(){
        data.popFront();
        return *this;
    }
    ZAssoc<K, T> &popBack(){
        data.popBack();
        return *this;
    }
    ZAssoc<K, T> &popFrontCount(unsigned cnt){
        data.popFrontCount(cnt);
        return *this;
    }

    ZAssoc<K, T> &concat(ZAssoc<K, T> in){
        data.concat(in.dat());
        return *this;
    }

    bool exists(K test){
        for(unsigned i = 0; i < size(); ++i){
            if(data[i].key == test)
                return true;
        }
        return false;
    }
    bool contains(T test){
        for(unsigned i = 0; i < size(); ++i){
            if(data[i].val == test)
                return true;
        }
        return false;
    }

    T &first(){
        return data.front().val;
    }
    T &last(){
        return data.back().val;
    }
    T &firstKey(){
        return data.front().key;
    }
    T &lastKey(){
        return data.back().key;
    }

    bool empty(){
        return data.empty();
    }

    unsigned size(){
        return data.size();
    }
    ZArray<Data> &dat(){
        return data;
    }

private:
    ZArray<Data> data;
    K empk;
};

}

#endif // ZASSOC_H
