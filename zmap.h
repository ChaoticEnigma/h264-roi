#ifndef ZMAP_H
#define ZMAP_H

#include "zarray.h"

namespace LibChaos {

template <class K, class T> class ZMap {
public:
    struct Data {
        K key;
        T val;
    };

    ZMap(){}
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
