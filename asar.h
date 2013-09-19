#ifndef ASSOCARRAY_H
#define ASSOCARRAY_H

#include <iostream>
#include <vector>
#include <string>
//#include "zstring.h"

namespace LibChaos {

//class ZString;
//template <class T> class AssocArray;

//typedef AssocArray<std::string> StrAsAr;
//typedef AssocArray<ZString> AsArZ;

template <class T> class AssocArray {
    public:
        AssocArray(){}
        T &at(int numindex){
            if(numindex < (int)data.size())
                return data[numindex].value;
            for(unsigned i = 0; i < data.size(); ++i){
                if(data[i].numindex == numindex)
                    return data[i].value;
            }
            data.push_back(Data("", numindex, T()));
            return data[data.size() - 1].value;
        }
        T &operator[](int numindex){
            return at(numindex);
        }
        T &at(std::string index){
            for(unsigned i = 0; i < data.size(); ++i){
                if(data[i].index == index)
                    return data[i].value;
            }
            data.push_back(Data(index, data.size(), T()));
            return data[data.size() - 1].value;
        }
        T &operator[](std::string index){
            return at(index);
        }
        // Incomplete
        //T &operator=(AssocArray<T> in){
        //    data.clear();
        //    for(unsigned i = 0; i < in.size(); ++i){
        //        push(in[i]);
        //    }
        //}
        std::string &getIndex(int numindex){
            if(numindex < (int)data.size())
                return data[numindex].index;
            for(unsigned i = 0; i < data.size(); ++i){
                if(data[i].numindex == numindex)
                    return data[i].index;
            }
            data.push_back(Data("", numindex, T()));
            return data[data.size() - 1].index;
        }
        std::string &I(int numindex){
            return getIndex(numindex);
        }
        void push(T value){
            operator[](size()) = value;
        }
        bool exists(std::string key){
            for(unsigned i = 0; i < data.size(); ++i){
                if(data[i].index == key)
                    return true;
            }
            return false;
        }
        AssocArray<T> &pop(std::string key){
            for(unsigned i = 0; i < data.size(); ++i){
                if(data[i].index == key){
                    pop(i);
                    i = 0;
                }
            }
            return *this;
        }
        AssocArray<T> &pop(unsigned index){
            data.erase(data.begin() + index);
            return *this;
        }
        AssocArray<T> &popCount(unsigned cnt){
            for(unsigned i = 0; i < cnt; ++i){
                pop(0);
            }
            return *this;
        }
        AssocArray<T> &pushFront(T in){
            typename std::vector<Data>::iterator it;
            it = data.begin();
            data.insert(it, in);
            return *this;
        }
        AssocArray<T> &clean(){ // Remove empty entries
            std::vector<Data> tmp;
            for(unsigned i = 0; i < data.size(); ++i){
                Data tmpem = data[i];
                if(!(tmpem.value == ""))
                    tmp.push_back(Data(tmpem.index, i, tmpem.value));
            }
            data = tmp;
            return *this;
        }
        unsigned size(){
            return data.size();
        }
        AssocArray<T> shift(){
            std::vector<Data> tmp;
            for(unsigned i = 1; i < data.size(); ++i)
                tmp.push_back(Data(data[i].index, i, data[i].value));
            data = tmp;
            return *this;
        }
        AssocArray<T> &append(AssocArray<T> in){
            for(unsigned i = 0; i < in.size(); ++i){
                data.push_back(Data(in.I(i), data.size(), in[i]));
            }
            return *this;
        }
        bool contains(T item){
            for(unsigned i = 0; i < data.size(); ++i)
                if(data[i].value == item)
                    return true;
            return false;
        }
        T &last(){
            return data[data.size() - 1].value;
        }
        bool empty(){
            if(data.size() <= 0)
                return true;
            return false;
        }
        void clear(){
            data.clear();
        }
    private:
        struct Data {
            //Data();
            Data(std::string inx, int ninx, T val) : index(inx), numindex(ninx), value(val){}
            std::string index;
            int numindex;
            T value;
        };
        std::vector<Data> data;
};

typedef AssocArray<std::string> AsArStr;

} // namespace LibChaos

// #include "asar.tpp"

#endif // ASSOCARRAY_H
