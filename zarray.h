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
    ZArray(T *in){
        data = (T *) malloc(0 * sizeof(T));
        datlen = 0;
        for(long i = 0; i < (sizeof(in)/sizeof(T)); ++i)
            push_back(in[i]);
    }
    ZArray(const T *in){
        data = (T *) malloc(0 * sizeof(T));
        datlen = 0;
        int len = sizeof(in);
        std::cout << len << " " << in << std::endl;
        for(long i = 0; i < len; ++i){
            std::cout << in[i] << std::endl;
            push_back(in[i]);
        }
        return;
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

    const T *c_style(){
        T tmp[datlen+1];
        for(unsigned long i = 0; i < datlen; ++i)
            tmp[i] = data[i];
        tmp[datlen] = '\0';
        return (const T *)tmp;
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
