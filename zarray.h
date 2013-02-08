#ifndef ZARRAY_H
#define ZARRAY_H

#include <cstdlib>

// ZArray v0.1 //

template <class T> class ZArray {
public:
    ZArray(){
        data = (T *) malloc(0 * sizeof(T));
        datlen = 0;
    }

    ~ZArray(){
        free(data);
    }

    void push_back(T val){
        datlen++;
        data = (T *) realloc(data, datlen * sizeof(T));
        data[datlen-1] = val;
    }

    void concat(ZArray<T> in){
        for(unsigned long i = 0; i < in.size(); ++i){
            push_back(in[i]);
        }
    }

    T &operator[](long num){ return data[num]; }

    bool operator==(ZArray<T> in){
        if(in.size() != size())
            return false;
        for(unsigned long i = 0; i < in.size(); ++i){
            if(in[i] != data[i])
                return false;
        }
        return true;
    }

    void clear(){
        free(data);
        data = (T *) malloc(0 * sizeof(T));
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
