/*******************************************************************************
**                                  LibChaos                                  **
**                                  zarray.h                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZARRAY_H
#define ZARRAY_H

#include "ztypes.h"
#include "zhash.h"
#include "zallocator.h"
#include "yindexedaccess.h"
#include "ypushpopaccess.h"
#include "ziterable.h"
#include "ziterator.h"

#include <initializer_list>

#define ZARRAY_INITIAL_CAPACITY 16

namespace LibChaos {

//! Automatically-resizing array container.
//! ZArray push/pop paradigm is FILO
template <typename T> class ZArray : public YIndexedAccess<T>, public YPushPopAccess<T>, public ZIterable<T> {
public:
    enum {
        none = ZU64_MAX
    };

public:
    ZArray(ZAllocator<T> *alloc = new ZAllocator<T>) : _alloc(alloc), _data(nullptr), _size(0), _realsize(0){
        reserve(ZARRAY_INITIAL_CAPACITY);
    }
    ZArray(const T *raw, zu64 size) : ZArray(){
        reserve(size);
        for(zu64 i = 0; i < size; ++i)
            _alloc->construct(&_data[i], raw[i]);
        _size = size;
    }
    ZArray(const T &first) : ZArray(){
        _alloc->construct(_data, first);
        _size = 1;
    }
    ZArray(const ZArray<T> &other) : ZArray(){
        reserve(other._size);
        for(zu64 i = 0; i < other._size; ++i)
            _alloc->construct(&_data[i], other[i]);
        _size = other._size;
    }
    ZArray(std::initializer_list<T> ls) : ZArray(){
        reserve(ls.size());
        zu64 i = 0;
        for(auto item = ls.begin(); item < ls.end(); ++item){
            _alloc->construct(&_data[i], *item);
            ++i;
        }
        _size = ls.size();
    }

    ~ZArray(){
        clear();
        delete _alloc;
    }

    void clear(){
        _alloc->destroy(_data, _size); // Destroy objects
        _alloc->dealloc(_data); // Delete memory
        _size = 0;
        _data = nullptr;
    }

    ZArray<T> &assign(const ZArray<T> &other){
        _alloc->destroy(_data, _size); // Destroy contents
        reserve(other.size()); // Make space
        for(zu64 i = 0; i < other.size(); ++i)
            _alloc->construct(&_data[i], other[i]); // Copy objects
        _size = other.size();
        return *this;
    }
    inline ZArray<T> &operator=(const ZArray<T> &other){ return assign(other); }

    void swap(ZArray<T>& other){
        ZAllocator<T> *alloc = _alloc;
        zu64 size =_size;
        zu64 realsize = _realsize;
        T *data = _data;

        _alloc = alloc;
        _size = other._size;
        _realsize = other._realsize;
        _data = other._data;

        other._alloc = alloc;
        other._size = size;
        other._realsize = realsize;
        other._data = data;
    }

    bool equals(const ZArray<T> &other) const {
        if(size() != other.size())
            return false;
        for(zu64 i = 0; i < size(); ++i){
            if(!(operator[](i) == other[i]))
                return false;
        }
        return true;
    }
    inline bool operator==(const ZArray<T> &other) const { return equals(other); }
    inline bool operator!=(const ZArray<T> &other) const { return !equals(other); }

    inline T &at(zu64 index){ return _data[index]; }
    inline T &operator[](zu64 index){ return at(index); }

    inline const T &at(zu64 index) const { return _data[index]; }
    inline const T &operator[](zu64 index) const { return at(index); }

    // Resize: chnage logical number of elements
    ZArray<T> &resize(zu64 size, const T &value = T()){
        reserve(size);
        if(size > _size){
            _alloc->construct(_data + _size, value, size - _size); // Construct new objects
        } else if(size < _size){
            _alloc->destroy(_data + size, _size - size); // Destroy extra objects
        }
        _size = size;
        return *this;
    }

    // Resize the real buffer (IMPORTANT: this is the only place memory is allocated)
    // If new size is larger, adds uninitialized space for more elements (subsequent resizes may not have to reallocate)
    // Never reallocates a smaller buffer! resize() assumes the new buffer is never smaller so it can destroy objects
    void reserve(zu64 size){
        if(size > _realsize){
            zu64 newsize = MAX(_size * 2, size);
            T *data = _alloc->alloc(newsize);
            _alloc->rawcopy(_data, data, _size); // Copy data to new buffer
            _alloc->dealloc(_data); // Delete old buffer without calling destructors
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

    // Insert <value> at <pos>, shifting subsequent elements
    ZArray<T> &insert(zu64 pos, const T &value){
        reserve(_size + 1);
        _alloc->rawmove(_data + pos, _data + pos + 1, _size - pos);
        _alloc->construct(_data + pos, value);
        ++_size;
        return *this;
    }

    ZArray<T> &pushBack(const T &value){
        reserve(_size + 1);
        _alloc->construct(_data + _size, value);
        ++_size;
        return *this;
    }
    ZArray<T> &pushFront(const T &value){
        return insert(0, value);
    }
    inline void push(const T &value){ pushBack(value); }

    ZArray<T> &erase(zu64 index, zu64 count = 1){
        _alloc->destroy(_data + index, count);
        _alloc->rawmove(_data + index + count, _data + index, _size - index - count);
        _size -= count;
        return *this;
    }

    T &peek(){
        return back();
    }
    const T &peek() const {
        return back();
    }

    ZArray<T> &remove(zu64 index){
        return erase(index);
    }
    ZArray<T> &popFront(){
        return remove(0);
    }
    ZArray<T> &popFrontCount(zu64 conut){
        return erase(0, conut);
    }
    ZArray<T> &popBack(){
        return resize(_size - 1);
    }
    inline void pop(){ popBack(); }

    ZArray<T> &append(const ZArray<T> &in){
        reserve(_size + in.size());
        for(zu64 i = 0; i < in.size(); ++i)
            _alloc->construct(_data + _size + i, in[i]);
        _size += in.size();
        return *this;
    }

    ZArray<T> concat(const ZArray<T> &in) const {
        ZArray<T> tmp(*this);
        tmp.append(in);
        return tmp;
    }

    ZArray<T> &reverse(){
        ZArray<T> tmp;
        tmp.reserve(_size);
        for(zu64 i = _size; i > 0; --i)
            tmp.pushBack(operator[](i-1));
        assign(tmp);
        return *this;
    }

    bool contains(const T &test) const {
        for(zu64 i = 0; i < _size; ++i){
            if(_data[i] == test)
                return true;
        }
        return false;
    }
    zu64 find(const T &test) const {
        for(zu64 i = 0; i < _size; ++i){
            if(_data[i] == test)
                return i;
        }
        return none;
    }

    ZIterator<T> iterator(){
        return ZIterator<T>(new ZArrayAccessor(this));
    }

    inline T &front(){ return _data[0]; }
    inline const T &front() const { return _data[0]; }
    inline T &back(){ return _data[_size - 1]; }
    inline const T &back() const { return _data[_size - 1]; }

    inline bool isEmpty() const { return (_size == 0); }
    inline bool empty() const { return isEmpty(); }

    inline zu64 size() const { return _size; }
    inline zu64 realsize() const { return _realsize; }
    inline zu64 capacity() const { return realsize(); }

    inline T *ptr() const { return _data; }
    inline T *raw() const { return ptr(); }

private:
    class ZArrayAccessor : public ZAccessor<T> {
    public:
        ZArrayAccessor(ZArray<T> *array) : _array(array), _pos(0){}

        bool atEnd() const {
            return _pos >= _size;
        }

        void forward(){
            ++_pos;
        }
        void back(){
            --_pos;
        }

        T &current(){
            return _array->operator[](_pos);
        }

    private:
        ZArray<T> *_array;
        zu64 _pos;
    };

private:
    ZAllocator<T> *_alloc;
    T *_data;
    zu64 _size;
    zu64 _realsize;
};

//ZHASH_USER_SPECIALIAZATION(ZArray<T>, (ZArray<T> array), (), {})

//template <typename T, ZHashBase::hashMethod M> class ZHash<ZArray<T>, M> : public ZHashMethod<M> {
//public:
//    ZHash(const ZArray<T> &array) : ZHashMethod<M>(){
//        for(zu64 i = 0; i < array.size(); ++i){
//            ZHash<T, M>(array[i])
//        }
//    }
//};
//template <> class ZHash<TYPE, ZHashBase::defaultHash> : public ZHashMethod<ZHashBase::defaultHash> {
//public:
//    ZHash(const ZArray<T> &array) : ZHashMethod<ZHashBase::defaultHash>(){
//
//    }
//};

}

#endif // ZARRAY_H
