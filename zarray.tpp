#ifndef BUILDING
    #include "zarray.h"
#endif

template <class T>
ZArray<T>::ZArray(){
    data = (T **) malloc(0 * sizeof(T *));
    datlen = 0;
}

template <class T>
void ZArray<T>::push_back(T val){
    datlen++;
    data = (T **) realloc(data, datlen * sizeof(T *));
    data[datlen-1] = val;
}

template <class T>
T &ZArray<T>::operator[](long num){
    return data[num];
}
