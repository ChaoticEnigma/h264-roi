#ifndef ZARRAY_H
#define ZARRAY_H

#include <vector>

#include "ztypes.h"

namespace LibChaos {

template <typename T> class ZArrayV1 {
public:
    ZArrayV1(){}
    ~ZArrayV1(){}

    ZArrayV1<T> &operator=(ZArrayV1<T> arr){
        data = arr.dat();
        return *this;
    }

    T &at(zu64 index){
        if(index < size()){
            return data[index];
        } else {
            //T tmp = {};
            //T temp[1] = {};
            //empv = temp[0];
            return empv;
        }
    }
    T &operator[](zu64 index){
        return at(index);
    }

    ZArrayV1<T> &push(T value){
        data.push_back(value);
        return *this;
    }

    ZArrayV1<T> &erase(zu64 index, zu64 count){
        data.erase(data.begin() + index, data.begin() + index + count);
        return *this;
    }
    ZArrayV1<T> &erase(zu64 index){
        return erase(index, 1);
    }

    ZArrayV1<T> &pop(zu64 index){
        return erase(index);
    }
    ZArrayV1<T> &popFront(){
        return pop(0);
    }
    ZArrayV1<T> &popBack(){
        if(!empty()){
            data.pop_back();
        }
        return *this;
    }
    ZArrayV1<T> &popFrontCount(unsigned conut){
        return erase(0, conut);
    }

    ZArrayV1<T> &pushFront(T in){
        typename std::vector<T>::iterator it;
        it = data.begin();
        data.insert(it, in);
        return *this;
    }

    ZArrayV1<T> &concat(ZArrayV1<T> in){
        for(unsigned i = 0; i < in.size(); ++i){
            data.push_back(in[i]);
        }
        return *this;
    }

//    ZArrayV1<T> &clean(){
//        for(long int i = 0; i < size(); ++i){
//            if(data[i] == T()){
//                pop(i);
//                i = -1;
//            }
//        }
//        return *this;
//    }

//    bool contains(T item){
//        for(unsigned i = 0; i < data.size(); ++i)
//            if(data[i] == item)
//                return true;
//        return false;
//    }

    T &front(){
        return data.front();
    }
    T &back(){
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
    zu64 size() const {
        return data.size();
    }

private:
    std::vector<T> data;
    T empv;
};

}

#include "zarray2.h"

namespace LibChaos {

//template <typename T> class ZArray : public ZArrayV1<T> {};
template <typename T> using ZArray = ZArrayV1<T>;

//template <typename T> class ZArray : public ZArrayV2<T> {};

}

#endif // ZARRAY_H
