#ifndef ZARRAY_H
#define ZARRAY_H

#include <cstdlib>
#include <iostream>

// ZArray v0.1 //

template <class T> class ZArray {
public:
    ZArray(){
        data = (T *) malloc(0 * sizeof(T));
        datlen = 0;
    }
    ZArray(ZArray<T> &arr){
        data = (T *) malloc(arr.size() * sizeof(T));
        datlen = arr.size();
        data = arr.c_style();
    }
    ZArray(T *in, unsigned long length){
        data = (T *) malloc(0 * sizeof(T));
        datlen = 0;
        for(unsigned long i = 0; i < length; ++i)
            push_back(in[i]);
    }
    ZArray(const T *in, unsigned long length){
        data = (T *) malloc(0 * sizeof(T));
        datlen = 0;
        for(unsigned long i = 0; i < length; ++i)
            push_back(in[i]);
    }

    ~ZArray(){
        free(data);
    }

    void push_back(T val){
        datlen++;
        data = (T *) realloc(data, datlen * sizeof(T));
        data[datlen-1] = val;
    }
    void drop_back(){
        datlen--;
        data = (T *) realloc(data, datlen * sizeof(T));
    }

    ZArray<T> concat(ZArray<T> in){
        for(unsigned long i = 0; i < in.size(); ++i){
            push_back(in[i]);
        }
        return *this;
    }

    ZArray<T> subarr(long pos, long len){
        ZArray<T> tmp;
        for(long i = 0; i < len && pos+i < size(); ++i)
            tmp.push_back(data[pos+i]);
        return tmp;
    }

    T &operator[](long num){ return data[num]; }

    void operator=(ZArray<T> in){
        clear();
        concat(in);
    }

    bool operator==(ZArray<T> in){
        if(in.size() != size())
            return false;
        for(unsigned long i = 0; i < in.size(); ++i){
            if(in[i] != data[i])
                return false;
        }
        return true;
    }

    T *c_style(){
        return data;
    }

    void clear(){
        data = (T *) realloc(data, 0 * sizeof(T));
    }

    unsigned long size(){ return datlen; }
private:
    struct Data {
        T value;
    };
    T *data;
    unsigned long datlen;
};

//#include "zarray.tpp"

#endif // ZARRAY_H
