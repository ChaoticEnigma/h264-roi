#ifndef ZARRAY_H
#define ZARRAY_H

#include <cstdlib>
#include <iostream>

// ZArray v0.1 //

template <class T, class Tsize = int> class ZArray {
public:
    ZArray(){
        data = (T *) malloc(0 * sizeof(T));
        datlen = 0;
    }
    ZArray(const ZArray<T> &arr){
        data = (T *) malloc(arr.size() * sizeof(T));
        datlen = arr.size();
        //data = arr.c_style();
        memcpy(data, arr.c_style(), datlen);
    }
    ZArray(T *in, Tsize length){
        data = (T *) malloc(0 * sizeof(T));
        datlen = 0;
        for(Tsize i = 0; i < length; ++i)
            push_back(in[i]);
    }
    ZArray(const T *in, Tsize length){
        data = (T *) malloc(0 * sizeof(T));
        datlen = 0;
        for(Tsize i = 0; i < length; ++i)
            push_back(in[i]);
    }

    ~ZArray(){
        free(data);
    }

    void push_back(T val){ // Note: Modifies Current Context
        datlen++;
        data = (T *) realloc(data, datlen * sizeof(T));
        data[datlen-1] = val;
    }
    void drop_back(){ // Note: Modifies Current Context
        datlen--;
        data = (T *) realloc(data, datlen * sizeof(T));
    }

    ZArray<T> concat(ZArray<T> in){ // Note: Modifies Current Context
        for(Tsize i = 0; i < in.size(); ++i){
            push_back(in[i]);
        }
        return *this;
    }

    ZArray<T> subarr(Tsize pos, Tsize len){
        ZArray<T> tmp;
        for(Tsize i = 0; i < len && pos+i < size(); ++i)
            tmp.push_back(data[pos+i]);
        return tmp;
    }

    T &operator[](Tsize num){ return data[num]; }

    void operator=(ZArray<T> in){
        clear();
        concat(in);
    }

    bool operator==(ZArray<T> in){
        if(in.size() != size())
            return false;
        for(Tsize i = 0; i < in.size(); ++i){
            if(in[i] != data[i])
                return false;
        }
        return true;
    }

    bool operator!=(ZArray<T> in){
        if(operator==(in))
            return false;
        return true;
    }

    T *c_style() const {
        return data;
    }

    void clear(){ // Note: Modifies Current Context
        data = (T *) realloc(data, 0);
        datlen = 0;
    }

    Tsize size() const { return datlen; }

private:
    struct Data {
        T value;
    };
    T *data;
    Tsize datlen;
};

//#include "zarray.tpp"

#endif // ZARRAY_H
