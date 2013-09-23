#ifndef ZARRAY_H
#define ZARRAY_H

#include <vector>

namespace LibChaos {

template <class T> class ZArray {
public:
    ZArray(){}
    ~ZArray(){}

    ZArray<T> &operator=(ZArray<T> arr){
        data = arr.dat();
        return *this;
    }

    T &at(unsigned index){
        if(index < size()){
            return data[index];
        } else {
            //T tmp = {};
            T temp[1] = {};
            empv = temp[0];
            return empv;
        }
    }
    T &operator[](unsigned index){
        return at(index);
    }

    ZArray<T> &push(T value){
        data.push_back(value);
        return *this;
    }

    ZArray<T> &pop(unsigned index){
        if(index < size()){
            data.erase(data.begin() + index);
        }
        return *this;
    }
    ZArray<T> &popFront(){
        if(!empty()){
            pop(0);
        }
        return *this;
    }
    ZArray<T> &popBack(){
        if(!empty()){
            data.pop_back();
        }
        return *this;
    }
    ZArray<T> &popFrontCount(unsigned cnt){
        for(unsigned i = 0; i < cnt; ++i){
            popFront();
        }
        return *this;
    }

    ZArray<T> &pushFront(T in){
        typename std::vector<T>::iterator it;
        it = data.begin();
        data.insert(it, in);
        return *this;
    }

    ZArray<T> &concat(ZArray<T> in){
        for(unsigned i = 0; i < in.size(); ++i){
            data.push_back(in[i]);
        }
        return *this;
    }

    ZArray<T> &clean(){
        for(long int i = 0; i < size(); ++i){
            if(data[i] == T()){
                pop(i);
                i = -1;
            }
        }
        return *this;
    }

    bool contains(T item){
        for(unsigned i = 0; i < data.size(); ++i)
            if(data[i] == item)
                return true;
        return false;
    }

    T &first(){
        return data.front();
    }
    T &last(){
        return data.back();
    }

    bool empty(){
        if(data.size() <= 0)
            return true;
        return false;
    }
    void clear(){
        data.clear();
    }

    std::vector<T> &dat(){
        return data;
    }
    unsigned size(){
        return data.size();
    }

private:
    std::vector<T> data;
    T empv;
};

}

#endif // ZARRAY_H
