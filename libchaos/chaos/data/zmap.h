/*****************************************
**               LibChaos               **
**               zmap.h                 **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZMAP_H
#define ZMAP_H

#include "zarray.h"

namespace LibChaos {

template <class K, class T> class ZMap {
public:
    enum {
        none = (zu64)-1
    };

public:
    struct Data {
        K key;
        T val;
    };

    ZMap(){}
    ZMap(std::initializer_list<Data> list) : data(list){}

    ~ZMap(){}

    T &at(K key_){
        for(zu64 i = 0; i < data.size(); ++i){
            if(data[i].key == key_)
                return data[i].val;
        }
        data.push({ key_, T() });
        return at(key_);
    }
    T &operator[](K key_){
        return at(key_);
    }

    Data &position(zu64 index){
        return data[index];
    }

    ZMap<K, T> &push(K key_, T value){
        data.push({ key_, value });
        return *this;
    }

    ZMap<K, T> &combine(ZMap<K, T> in){
        for(zu64 i = 0; i < in.size(); ++i){
            push(in.position(i).key, in.position(i).val);
        }
        return *this;
    }

    void erase(K test){
        data.erase(indexOf(test));
    }

    zu64 indexOf(K test) const {
        for(zu64 i = 0; i < size(); ++i){
            if(data[i].key == test)
                return i;
        }
        return none;
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

    bool empty() const {
        return data.empty();
    }

    unsigned size() const {
        return data.size();
    }
    ZArray<Data> &dat(){
        return data;
    }

private:
    ZArray<Data> data;
};

}

#endif // ZMAP_H
