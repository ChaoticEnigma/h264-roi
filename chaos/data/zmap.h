/*****************************************
**               LibChaos               **
**               zmap.h                 **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZMAP_H
#define ZMAP_H

#include "zallocator.h"
#include "zhashable.h"

namespace LibChaos {

template <class K, class T> class ZMap {
public:
    enum { none = ZU64_MAX };
public:
    struct MapSet {
        K key;
        T value;
    };

    ZMap(ZAllocator<K> kalloc = new ZAllocator<K>(), ZAllocator<T> talloc = new ZAllocator<T>()) : _alloc(new ZAllocator<MapData>()), _kalloc(kalloc), _talloc(talloc), _data(nullptr), _size(0), _realsize(0){}
    ZMap(std::initializer_list<MapSet> list) : ZMap(){
        resize(list.size());
        for(auto item = list.begin(); item < list.end(); ++item){
            add(item->key, item->value);
        }
    }
    ~ZMap(){

    }

    void add(K &key, T &value){
        zu64 hash =
        MapData set;
        set.key = _kalloc.alloc(1);
        _kalloc.construct(set.key, 1, key);
        set.val = _talloc.alloc(1);
        _talloc.construct(set.val, 1, value);
        ++_size;
    }

//    T &get(K key_){

//    }
//    T &operator[](K key_){

//    }

//    ZMap<K, T> &push(K key_, T value){
//        _data.push({ key_, value });
//        return *this;
//    }

//    ZMap<K, T> &combine(ZMap<K, T> in){
//        for(zu64 i = 0; i < in.size(); ++i){
//            push(in.position(i).key, in.position(i).val);
//        }
//        return *this;
//    }

    // Resize the real buffer (IMPORTANT: this is the only place memory is allocated)
    // If new size is larger, adds uninitialized space for more elements (subsequent resizes may not have to reallocate)
    // Never reallocates a smaller buffer! resize() assumes the new buffer is never smaller so it can destroy objects
    void resize(zu64 size){
        if(size > _realsize){
//            zu64 newsize = _realsize * 2; // Double old buffer
            zu64 newsize = 1;
            while(newsize < size)
                size <<= 1; // Get next power of 2
            T *data = _alloc.alloc(newsize);
            _alloc.rawcopy(_data, data, _size); // Copy data to new buffer
            _alloc.dealloc(_data); // Delete old buffer without calling destructors
            _data = data;
            _realsize = newsize;
        }
    }

    void erase(K test){

    }

    zu64 indexOf(K test) const {
        return none;
    }

    bool exists(K test){
        return false;
    }
    bool contains(T test){
        return false;
    }

    zu64 size() const {
        return _size;
    }
    bool isEmpty() const {
        return (_data == nullptr) || _size == 0;
    }

protected:
    struct MapData {
        K *key;
        T *val;
    };

    ZAllocator<MapData> _alloc;
    ZAllocator<K> _kalloc;
    ZAllocator<T> _talloc;
    MapData *_data;
    zu64 _size;
    zu64 _realsize;
};

#endif // ZMAP_H
