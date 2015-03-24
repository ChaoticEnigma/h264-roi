/*******************************************************************************
**                                  LibChaos                                  **
**                                   zmap.h                                   **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#ifndef ZMAP_H
#define ZMAP_H

#include "zallocator.h"
#include "zhashable.h"
#include "zhash.h"
#include "zexception.h"

namespace LibChaos {

// For the purposes of this class, keys that evaluate equal must have the same hashes, and vice-versa
template <class K, class T> class ZMap {
public:
    enum { none = ZU64_MAX };
public:
    struct MapSet {
        K key;
        T value;
    };

    // If key is null, the entry is empty
    // If key is null AND the value is not null, the value was deleted, and value is not a valid pointer
    struct MapData {
        zu64 hash;
        K *key;
        T *value;
    };

    ZMap(ZAllocator<K> *kalloc = new ZAllocator<K>(), ZAllocator<T> *talloc = new ZAllocator<T>())
      : _alloc(new ZAllocator<MapData>()), _kalloc(kalloc), _talloc(talloc), _data(nullptr), _size(0), _realsize(0), _factor(0.5){
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

    T &add(const K &key, const T &value){
        // Resize if over load factor
        if(((float)_size / (float)_realsize) >= _factor){
            resize(_realsize << 1);
        }

        zu64 hash = ZHash<K>(key).hash();
        zu64 ipos = hash % _realsize;
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = (ipos + i) % _realsize;
            // Look for somewhere to put the value
            if(_data[pos].key == nullptr){
                // Element is unset or deleted
                _data->hash = hash;
                _data[pos].key = _kalloc->alloc(1);
                _kalloc->construct(_data[pos].key, 1, key);
                _data[pos].value = _talloc->alloc(1);
                _talloc->construct(_data[pos].value, 1, value);
                ++_size;
                return *_data[pos].value;
            } else if(_data[pos].hash == hash){
                // Compare the actual key - may be non-trivial
                if(*_data[pos].key == key){
                    // Reassign key and value in existing element
                    _kalloc->destroy(_data[pos].key, 1);
                    _kalloc->construct(_data[pos].key, 1, key);
                    _talloc->destroy(_data[pos].value, 1);
                    _talloc->construct(_data[pos].value, 1, value);
                    return *_data[pos].value;
                }
            }
        }
        throw ZException(ZString("Unable to add element to map ") + hash);
    }
    inline void push(const K &key, const T &value){ add(key, value); }

    void remove(const K &key){
        zu64 hash = ZHash<K>(key).hash();
        zu64 ipos = hash % _realsize;
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = (ipos + i) % _realsize;
            // Look for the key
            if(_data[pos].key == nullptr){
                if(_data[pos].value == nullptr ){
                    // End of chain, stop searching
                    break;
                }
            } else {
                if(_data[pos].hash == hash){
                    // Compare the actual key - may be non-trivial
                    if(*_data[pos].key == key){
                        // Found it, delete it
                        _kalloc->destroy(_data[pos].key, 1);
                        _data[pos].key = nullptr;
                        _talloc->destroy(_data[pos].value, 1);
                        _data[pos].value = (T*)1; // Not null
                        --_size;
                    }
                }
            }
        }
    }

    T &get(const K &key){
        zu64 hash = ZHash<K>(key).hash();
        zu64 start = hash % _realsize;
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = (start + i) % _realsize;
            // Look for the key
            if(_data[pos].key == nullptr){
                if(_data[pos].value == nullptr ){
                    // End of chain, stop searching
                    break;
                }
            } else {
                if(_data[pos].hash == hash){
                    // Compare the actual key - may be non-trivial
                    if(*_data[pos].key == key){
                        // Found it
                        return *_data[pos].value;
                    }
                }
            }
        }
        // Create a new, default constructed object
        return add(key, T());
    }
    inline T &operator[](const K &key){ return get(key); }

    const T &get(const K &key) const {
        zu64 hash = ZHash<K>(key).hash();
        zu64 ipos = hash % _realsize;
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = (ipos + i) % _realsize;
            // Look for the key
            if(_data[pos].hash == hash){
                if(_data[pos].key == nullptr){
                    if(_data[pos].value == nullptr ){
                        // End of chain, stop searching
                        break;
                    } else {
                        // Deleted entry, keep searching
                        continue;
                    }
                } else {
                    // Compare the actual key - may be non-trivial
                    if(*_data[pos].key == key){
                        return *_data[pos].value;
                    }
                }
            }
        }
        throw ZException("Key does not exist", __LINE__);
    }
    inline const T &operator[](const K &key) const { return get(key); }

    MapData &position(zu64 i){
        return _data[i];
    }

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

    zu64 size() const { return _size; }
    zu64 realSize() const { return _realsize; }

    bool isEmpty() const {
        return (_data == nullptr) || _size == 0;
    }

protected:
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
