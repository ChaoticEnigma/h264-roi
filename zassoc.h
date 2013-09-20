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

    T &at(K key){
        for(unsigned i = 0; i < data.size(); ++i){
            if(data[i].key == key)
                return data[i].val;
        }
        data.push(Data(key, T()));
        //T temp = {};
        //data.push({ key, temp });
        return last();
    }
    T &operator[](K key){
        return at(key);
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

    ZAssoc<K, T> &push(K key, T value){
        data.push(Data(key, value));
        //data.push({ key, value });
    }
    ZAssoc<K, T> &push(T value){
        push(K(), value);
    }
    ZAssoc<K, T> &pushFront(K key, T value){
        data.pushFront(Data(key, value));
        //data.pushFront({ key, value });
        return *this;
    }
    ZAssoc<K, T> &pushFront(T value){
        pushFront(K(), value);
        return *this;
    }

    ZAssoc<K, T> &pop(unsigned index){
        data.pop(index);
        return *this;
    }
    ZAssoc<K, T> &popAll(K key){
        for(unsigned i = 0; i < data.size(); ++i){
            if(data[i].key == key){
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
        return data.first().val;
    }
    T &last(){
        return data.last().val;
    }
    T &firstKey(){
        return data.first().key;
    }
    T &lastKey(){
        return data.last().key;
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
