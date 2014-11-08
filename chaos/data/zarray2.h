#ifndef ZARRAY2_H
#define ZARRAY2_H

#include "ztypes.h"
#include "zallocator.h"
#include "yindexedaccess.h"

#include <initializer_list>

namespace LibChaos {

// ZArray push/pop paradigm is FILO
template <typename T> class ZArray : public YIndexedAccess<T> {
public:
    ZArray() : _size(0), _realsize(0), _data(nullptr){
        reserve(4); // Initial capacity
    }
    ZArray(const T *raw, zu64 size) : ZArray(){
        reserve(size);
        for(zu64 i = 0; i < size; ++i)
            _alloc.construct(_data + i, 1, raw[i]);
        _size = size;
    }
    ZArray(const T &first) : ZArray(){
        _alloc.construct(_data, 1, first);
        _size = 1;
    }
    ZArray(const ZArray<T> &other) : ZArray(){
        reserve(other._size);
        for(zu64 i = 0; i < other._size; ++i)
            _alloc.construct(_data + i, 1, other[i]);
        _size = other._size;
    }
    ZArray(std::initializer_list<T> ls) : ZArray(){
        reserve(ls.size());
        zu64 i = 0;
        for(auto item = ls.begin(); item < ls.end(); ++item){
            _alloc.construct(_data + i, 1, *item);
            ++i;
        }
        _size = ls.size();
    }

    ~ZArray(){
        clear();
    }

    void clear(){
        _alloc.destroy(_data, _size); // Destroy objects
        _alloc.dealloc(_data); // Delete memory
        _size = 0;
        _data = nullptr;
    }

    ZArray<T> &operator=(const ZArray<T> &other){
        reserve(other.size());
        _alloc.destroy(_data, _size); // Destroy contents
        _size = other.size();
        for(zu64 i = 0; i < _size; ++i)
            _alloc.construct(_data + i, 1, other[i]); // Copy objects
        return *this;
    }

    bool operator==(const ZArray<T> &arr) const {
        if(size() != arr.size())
            return false;
        for(zu64 i = 0; i < size(); ++i){
            if(!(operator[](i) == arr[i]))
                return false;
        }
        return true;
    }
    bool operator!=(const ZArray<T> &arr) const {
        return !operator==(arr);
    }

    inline T &at(zu64 index){ return _data[index]; }
    inline T &operator[](zu64 index){ return at(index); }

    inline const T &at(zu64 index) const { return _data[index]; }
    inline const T &operator[](zu64 index) const { return at(index); }

    // Resize (IMPORTANT: this is the only place memory is allocated)
    ZArray<T> &resize(zu64 size, const T &value = T()){
        if(size > _realsize){
            reserve(size);
        }
        if(size > _size){
            _alloc.construct(_data + _size, size - _size, value); // Construct new objects
        } else if(size < _size){
            _alloc.destroy(_data, _size - size); // Destroy extra objects
        }
        _size = size;
        return *this;
    }

    // Resize the real buffer
    // If new size is larger, adds uninitialized space for more elements (subsequent resizes may not have to reallocate)
    // Otherwise does nothing
    void reserve(zu64 size){
        if(size > _realsize){
            zu64 newsize = MAX(_size * 2, size);
            T *data = _alloc.alloc(newsize);
            _alloc.rawcopy(_data, data, _size); // Copy data to new buffer
            _alloc.dealloc(_data); // Delete old buffer without calling destructors
            _data = data;
            _realsize = newsize;
        }
    }

    // Add <count> objects to array
    ZArray<T> &extend(zu64 count){
        return resize(_size + count);
    }
    // Remove <count> objects from array
    ZArray<T> &contract(zu64 count){
        return resize(_size - count);
    }

    ZArray<T> &pushBack(const T &value){
        reserve(_size + 1);
        _alloc.construct(_data + _size, 1, value);
        ++_size;
        return *this;
    }
    ZArray<T> &pushFront(const T &value){
        return insert(0, value);
    }
    inline ZArray<T> &push(const T &value){ return pushBack(value); }

    ZArray<T> &insert(zu64 pos, const T &value){
        reserve(_size + 1);
        _alloc.rawmove(_data + pos, _data + pos + 1, 1);
        _alloc.construct(_data + pos, 1, value);
        ++_size;
        return *this;
    }

    ZArray<T> &concat(const ZArray<T> &in){
        zu64 presize = _size;
        reserve(_size + in.size());
        for(zu64 i = 0; i < in.size(); ++i)
            _alloc.construct(_data + presize, i, in[i]);
        _size = _size + in.size();
        return *this;
    }

    ZArray<T> &erase(zu64 index, zu64 count){
        _alloc.destroy(_data + index, count);
        _alloc.rawmove(_data + index + count, _data + index, count);
        _size -= count;
        return *this;
    }
    inline ZArray<T> &erase(zu64 index){
        return erase(index, 1);
    }

    ZArray<T> &pop(zu64 index){
        return erase(index);
    }
    ZArray<T> &popFront(){
        return pop(0);
    }
    ZArray<T> &popBack(){
        return resize(_size - 1);
    }
    ZArray<T> &pop(){
        return resize(_size - 1);
    }
    ZArray<T> &popFrontCount(unsigned conut){
        return erase(0, conut);
    }

    bool contains(const T &test) const {
        for(zu64 i = 0; i < _size; ++i){
            if(_data[i] == test)
                return true;
        }
        return false;
    }

    T &front(){ return _data[0]; }
    const T &front() const { return _data[0]; }
    T &back(){ return _data[_size - 1]; }
    const T &back() const { return _data[_size - 1]; }

    bool empty() const { return (_size == 0); }
    bool isEmpty() const { return empty(); }

    inline zu64 size() const { return _size; }
    inline zu64 realsize() const { return _realsize; }
    inline zu64 capacity() const { return realsize(); }

    inline T *ptr() const { return _data; }
    inline T *raw() const { return ptr(); }

private:
    ZAllocator<T> _alloc;
    zu64 _size;
    zu64 _realsize;
    T *_data;
};

}

#endif // ZARRAY2_H
