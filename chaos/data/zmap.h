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

template <typename K, typename = void> class XMapKeyImpl {

};

template <typename K> class XMapKeyImpl<K, typename std::enable_if<std::is_pod<K>::value>::type> {
protected:
    typedef K keytype;
protected:
    void setKey(keytype *key, keytype *src){

    }
};

// For the purposes of this class, keys that evaluate equal must have the same hashes, and vice-versa
template <typename K, typename T> class ZMap : private XMapImpl {
public:
    enum { none = ZU64_MAX };
public:
    typedef zu64 maphash;
    struct MapSet {
        K key;
        T value;
    };
    // If key is null, the entry is empty
    // If key is null AND the value is not null, the value was deleted, and value is not a valid pointer
    // MapData flags:
    // 0x01 = empty entry
    // 0x02 = previously deleted entry
    struct MapData {
        zbyte flags;
        zu64 hash;
        K key;
        T value;
    };
public:
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
        if(_data != nullptr){
            for(zu64 i = 0; i < _realsize; ++i){
                if(_data[i].key != nullptr){
                    _kalloc->destroy(_data[i].key);
                    //_kalloc->dealloc(_data[i].key);
                    _talloc->destroy(_data[i].value);
                    //_talloc->dealloc(_data[i].value);
                }
            }
            _alloc->dealloc(_data);
        }
        delete _talloc;
        delete _kalloc;
        delete _alloc;
    }

    // Add entry with <key> and <value> to map, or change value of existing entry with <key>
    T &add(const K &key, const T &value){
        // Resize if over load factor
        if(((float)_size / (float)_realsize) >= _factor){
            resize(_realsize << 1);
        }

        zu64 hash = _getHash(key);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
            // Look for the key
            if(_data[pos].key == nullptr){
                // Entry is unset or deleted, insert new entry
                _data[pos].hash = hash;
                //_data[pos].key = _kalloc->alloc(1);
                _kalloc->construct(_data[pos].key, 1, key);
                //_data[pos].value = _talloc->alloc(1);
                _talloc->construct(_data[pos].value, 1, value);
                ++_size;
                return *_data[pos].value;
            } else if(_data[pos].hash == hash){
                // Compare the actual key - may be non-trivial
                if(*_data[pos].key == key){
                    // Reassign key and value in existing entry
                    _kalloc->destroy(_data[pos].key, 1);
                    _kalloc->construct(_data[pos].key, 1, key);
                    _talloc->destroy(_data[pos].value, 1);
                    _talloc->construct(_data[pos].value, 1, value);
                    return *_data[pos].value;
                }
            }
        }
        // Should only ever fail if resize failed and the map is full
        throw ZException(ZString("Unable to add element to map ") + hash);
    }
    inline T &push(const K &key, const T &value){ return add(key, value); }

    // Remove entry with <key> from map, if it exists
    void remove(const K &key){
        zu64 hash = _getHash(key);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
            if(_data[pos].key == nullptr){
                if(_data[pos].value == nullptr ){
                    // End of chain, stop searching
                    break;
                }
            } else if(_data[pos].hash == hash){
                // Compare the actual key - may be non-trivial
                if(*_data[pos].key == key){
                    // Found it, delete it
                    _kalloc->destroy(_data[pos].key, 1);
                    //_kalloc->dealloc(_data[pos].key);
                    _talloc->destroy(_data[pos].value, 1);
                    //_talloc->dealloc(_data[pos].value);
                    _data[pos].key = nullptr;
                    _data[pos].value = (T*)1; // Not null
                    --_size;
                }
            }
        }
    }

    // Get a reference to the entry with <key>, create entry if it doesn't exitst
    T &get(const K &key){
        zu64 hash = _getHash(key);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
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

    // Get a reference to the entry with <key>
    // Throws an exception if the entry doesn't exist
    const T &get(const K &key) const {
        zu64 hash = _getHash(key);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
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
        throw ZException("Key does not exist", __LINE__);
    }
    inline const T &operator[](const K &key) const { return get(key); }

    zu64 getPosition(const K &key){
        return 0;
    }

    // Resize the buffer (this is the only place buffer memory is allocated)
    // Buffer is resized to the next power of two that will hold <size> elements
    // The buffer is never made smaller
    void resize(zu64 size){
        if(size > _realsize){
            zu64 newsize = 1;
            while(newsize < size)
                newsize <<= 1; // Get next power of 2

            MapData *olddata = _data;
            zu64 oldsize = _realsize;

            _data = _alloc->alloc(newsize);
            _realsize = newsize;
            _size = 0;

            // Clear new entries
            for(zu64 i = 0; i < _realsize; ++i){
                _data[i].key = nullptr;
                _data[i].value = nullptr;
            }

            // Re-map old entries
            if(olddata != nullptr){
                for(zu64 i = 0; i < oldsize; ++i){
                    if(olddata[i].key != nullptr){
                        for(zu64 j = 0; j < _realsize; ++j){
                            zu64 pos = _getPos(olddata[i].hash, j);
                            if(_data[pos].key == nullptr){
                                _data[pos].hash = olddata[i].hash;
                                //_data[pos].key = olddata[i].key;
                                _kalloc->copy(&olddata[i].key, &_data[pos].key, 1);
                                //_data[pos].value = olddata[i].value;
                                _talloc->copy(&olddata[i].value, &_data[pos].value, 1);
                                ++_size;
                                break;
                            }
                        }
                        throw ZException("Fatal Error in addHashEntry");
                    }
                }
                _alloc->dealloc(olddata);
            }
        }
    }

    void erase(K test){

    }

    bool exists(K test){
        return false;
    }
    bool contains(T test){
        return false;
    }

    bool isEmpty() const {
        return (_data == nullptr) || _size == 0;
    }

    zu64 size() const { return _size; }
    zu64 realSize() const { return _realsize; }

    float factor() const { return _factor; }
    void setFactor(float factor){ _factor = factor; }

    // For debugging
    MapData &position(zu64 i){
        return _data[i];
    }

private:
    static zu64 _getHash(const K &key){
        return ZHash<K>(key).hash();
    }
    zu64 _getPos(zu64 hash, zu64 i) const {
        return ((hash % _realsize) + i) % _realsize;
    }

private:
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
