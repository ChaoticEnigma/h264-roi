/*****************************************
**               LibChaos               **
**               zmap.h                 **
**       (c) 2013 Zennix Studios        **
*****************************************/
#ifndef ZMAP_H
#define ZMAP_H

#include "zallocator.h"
#include "zhashable.h"
#include "zhash.h"
//#include "zexception.h"

namespace LibChaos {

template <class K, class T> class ZMap {
public:
    enum { none = ZU64_MAX };
public:
    struct MapSet {
        K key;
        T value;
    };

    ZMap(ZAllocator<K> *kalloc = new ZAllocator<K>(), ZAllocator<T> *talloc = new ZAllocator<T>()) : _alloc(new ZAllocator<MapData>()), _kalloc(kalloc), _talloc(talloc), _data(nullptr), _size(0), _realsize(0), _factor(0.5){
        resize(16);
    }
    ZMap(std::initializer_list<MapSet> list) : ZMap(){
        resize(list.size());
        for(auto item = list.begin(); item < list.end(); ++item){
            add(item->key, item->value);
        }
    }
    ~ZMap(){

    }

    void add(K &key, T &value){
        // Resize if over load factor
        if(((float)_size / (float)_realsize) >= _factor){
            resize(_realsize << 1);
        }

        zu64 hash = ZHash<K>(key).hash();
        zu64 ipos = hash % _realsize;
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = (ipos + i) % _realsize;
            if(_data[pos].key == nullptr){
                // Element is unset or deleted
                _data->hash = hash;
                _data[pos].key = _kalloc->alloc(1);
                _kalloc->construct(_data[pos].key, 1, key);
                _data[pos].value = _talloc->alloc(1);
                _talloc->construct(_data[pos].value, 1, value);
                ++_size;
                return;
            } else if(_data[pos].hash == hash){
                // Compare the actual key - may be non-trivial
                if(*_data[pos].key == key){
                    // Reassign value in existing element
                    _talloc->destroy(_data[pos].value, 1);
                    _talloc->construct(_data[pos].value, 1, value);
                }
            }
        }
        //throw ZException();
    }

    T &get(const K &key){
        zu64 hash = ZHash<K>(key).hash();
        zu64 ipos = hash % _realsize;
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = (ipos + i) % _realsize;
            if(_data[pos].hash == hash){
                // Compare the actual key - may be non-trivial
                if(*_data[pos].key == key){
                    return *_data[pos].value;
                }
            } else if(_data[pos].key == nullptr && _data[pos].value == nullptr){

            }
            if(_data[pos].key == nullptr){
                // Element is unset or deleted
                _data->hash = hash;
                _data[pos].key = _kalloc->alloc(1);
                _kalloc->construct(_data[pos].key, 1, key);
                _data[pos].value = _talloc->alloc(1);
                _talloc->construct(_data[pos].value, 1, value);
                ++_size;
                return;
            }
        }

    }
    inline T &operator[](const K &key){ return get(key); }

    const T &get(const K &key) const {
        zu64 hash = ZHash<K>(key).hash();
    }
    inline const T &operator[](const K &key) const { return get(key); }

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
            MapData *data = _alloc->alloc(newsize);
            _alloc->rawcopy(_data, data, _size); // Copy data to new buffer
            _alloc->dealloc(_data); // Delete old buffer without calling destructors
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
    // If key is null, the entry is empty
    // If key is null AND the value is not null, the value was deleted, and value is not a valid pointer
    struct MapData {
        zu64 hash;
        K *key;
        T *value;
    };

    ZAllocator<MapData> *_alloc;
    ZAllocator<K> *_kalloc;
    ZAllocator<T> *_talloc;
    MapData *_data;
    zu64 _size;
    zu64 _realsize;
    float _factor;
};

}

#endif // ZMAP_H
