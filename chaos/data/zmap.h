/*******************************************************************************
**                                  LibChaos                                  **
**                                   zmap.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZMAP_H
#define ZMAP_H

#include "zallocator.h"
#include "zhashable.h"
#include "zhash.h"
#include "zexception.h"
#include "zarray.h"
#include "zpointer.h"

#include <initializer_list>

#define ZMAP_DEFAULT_LOAD_FACTOR    0.5
#define ZMAP_INITIAL_CAPACITY       16

// MapData flags:
//#define ENTRY_EMPTY     0x00 // empty entry
#define ZMAP_ENTRY_VALID     0x01 // valid data in entry
#define ZMAP_ENTRY_DELETED   0x02 // previously deleted entry
// If ENTRY_VALID is set, ENTRY_DELETED is ignored

namespace LibChaos {

/*! Hash Map container.
 *  Unsequenced.
 *  Keys must be hashable and comparable.
 *  Keys that evaluate equal must have the same hashes, and vice-versa.
 */
template <typename K, typename T> class ZMap {
public:
    enum { none = ZU64_MAX };
public:
    typedef zu64 maphash;

    struct MapPair {
        K key;
        T value;
    };

    struct MapElement {
        zbyte flags;    // Some flags for unset and deleted
        MapElement *next;
        MapElement *prev;
        zu64 hash;
        K key;
        T value;
    };

    class ZMapIterator;

public:
    ZMap(float loadfactor = ZMAP_DEFAULT_LOAD_FACTOR, ZAllocator<MapElement> *alloc = new ZAllocator<MapElement>) :
            _alloc(alloc), _data(nullptr), _size(0), _realsize(0), _factor(loadfactor){
        resize(ZMAP_INITIAL_CAPACITY);
    }

    ZMap(std::initializer_list<MapPair> list) : ZMap(){
        resize(list.size());
        for(auto item = list.begin(); item < list.end(); ++item){
            add(item->key, item->value);
        }
    }

    ZMap(const ZMap &other) : _alloc(other._alloc), _data(nullptr), _head(nullptr), _tail(nullptr),
            _size(0), _realsize(0), _factor(other._factor){
        if(other._data != nullptr && other._size > 0){
            resize(other._size);
            for(auto it = other.begin(); it.more(); ++it){
                add(*it, other.get(*it));
            }
        }
    }

    ~ZMap(){
        MapElement *current = _head;
        while(current != nullptr){
            _kalloc.destroy(&current->key);
            _talloc.destroy(&current->value);
            current = current->next;
        }
        _alloc->dealloc(_data);
    }

    //! Add entry with \a key and \a value to map, or change value of existing entry with \a key.
    T &add(const K &key, const T &value){
        // Ensure enough space
        resize(_size + 1);

        zu64 hash = _getHash(key);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
            // Look for the key
            if(!(_data[pos].flags & ZMAP_ENTRY_VALID)){
                // Entry is unset or deleted, insert new entry
                _data[pos].hash = hash;
                _kalloc.construct(&_data[pos].key, key);
                _talloc.construct(&_data[pos].value, value);
                _data[pos].flags |= ZMAP_ENTRY_VALID; // Set valid bit
                _data[pos].flags &= ~ZMAP_ENTRY_DELETED; // Unset deleted bit
                _data[pos].prev = _tail;
                _data[pos].next = nullptr;
                if(!_head) _head = _data + pos; // Set head if unset
                if(_tail) _tail->next = _data + pos; // Point prev element to this element
                _tail = _data + pos; // Update tail
                ++_size;
                return _data[pos].value;
            } else if(_data[pos].hash == hash){
                // Compare the actual key - may be non-trivial
                if(_data[pos].key == key){
                    // Reassign key and value in existing entry
                    _kalloc.destroy(&_data[pos].key);
                    _kalloc.construct(&_data[pos].key, key);
                    _talloc.destroy(&_data[pos].value);
                    _talloc.construct(&_data[pos].value, value);
                    return _data[pos].value;
                }
            }
        }
        // Should only ever fail if resize failed and the map is full
        throw ZException(ZString("ZMap: Unable to add element to map ") + hash);
    }
    inline T &push(const K &key, const T &value){ return add(key, value); }

    //! Remove entry with \a key from map, if it exists.
    void remove(const K &key){
        zu64 hash = _getHash(key);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
            if(_data[pos].flags & ZMAP_ENTRY_VALID){
                // Valid data, compare hash
                if(_data[pos].hash == hash){
                    // Compare the actual key - may be non-trivial
                    if(_data[pos].key == key){
                        // Found it, delete it
                        _kalloc.destroy(&_data[pos].key);
                        _talloc.destroy(&_data[pos].value);
                        _data[pos].flags &= ~ZMAP_ENTRY_VALID; // Unset valid bit
                        _data[pos].flags |= ZMAP_ENTRY_DELETED; // Set deleted bit
                        if(_data[pos].prev) _data[pos].prev->next = _data[pos].next; // Point prev element to next
                        if(_data[pos].next) _data[pos].next->prev = _data[pos].prev; // Point next element to prev
                        if(_head == _data + pos) _head = _data[pos].next; // Move head if necessary
                        if(_tail == _data + pos) _tail = _data[pos].prev; // Move tail if necessary
                        --_size;
                        break;
                    }
                }
            } else if(!(_data[pos].flags & ZMAP_ENTRY_DELETED)){
                // If this is not a deleted entry, it is the end of the chain
                break;
            }
        }
    }

    //! Get a reference to the entry with \a key, create entry if it doesn't exitst.
    T &get(const K &key){
        zu64 hash = _getHash(key);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
            if(_data[pos].flags & ZMAP_ENTRY_VALID){
                // Look for the key
                if(_data[pos].hash == hash){
                    // Compare the actual key - may be non-trivial
                    if(_data[pos].key == key){
                        // Found it
                        return _data[pos].value;
                    }
                }
            } else if(!(_data[pos].flags & ZMAP_ENTRY_DELETED)){
                // If this is not a deleted entry, it is the end of the chain
                break;
            }
        }
        // Create a new, default constructed object
        return add(key, T());
    }
    inline T &operator[](const K &key){ return get(key); }

    /*! Get a reference to the entry with \a key.
     *  \throws Throws an exception if the entry doesn't exist
     */
    const T &get(const K &key) const {
        zu64 hash = _getHash(key);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
            if(_data[pos].flags & ZMAP_ENTRY_VALID){
                // Look for the key
                if(_data[pos].hash == hash){
                    // Compare the actual key - may be non-trivial
                    if(_data[pos].key == key){
                        // Found it
                        return _data[pos].value;
                    }
                }
            } else {
                if(!(_data[pos].flags & ZMAP_ENTRY_DELETED)){
                    // If this is not a deleted entry, it is the end of the chain
                    break;
                }
            }
        }
        throw ZException("ZMap: Key does not exist", __LINE__);
    }
    inline const T &operator[](const K &key) const { return get(key); }

    zu64 getPosition(const K &key){
        return 0;
    }

    /*! Resize the buffer (this is the only place buffer memory is allocated)
     *  Buffer is resized to the next power of two that will hold \a size elements
     *  The buffer is never made smaller
     */
    void resize(zu64 size){
        if(size > _realsize || ((float)size / (float)_realsize) >= _factor){
            zu64 newsize = 1;
            // Get a new buffer size that can hold <size> entries, staying under the load factor
            while(newsize < size || ((float)size / (float)newsize) >= _factor)
                newsize <<= 1;

            MapElement *olddata = _data;
            zu64 oldsize = _realsize;

            _realsize = newsize;
            _data = _alloc->alloc(_realsize);

            // Clear new entries
            for(zu64 i = 0; i < _realsize; ++i){
                _data[i].flags = 0; // Clean flags
                //_data[i].next = nullptr;
            }

            if(olddata != nullptr){
                //MapData *last = nullptr;
                // Re-map old entries
                for(zu64 i = 0; i < oldsize; ++i){
                    // Map only non-empty entries
                    if(olddata[i].flags & ZMAP_ENTRY_VALID){
                        for(zu64 j = 0; j < _realsize; ++j){
                            zu64 pos = _getPos(olddata[i].hash, j);
                            // Find first empty entry
                            if(!(_data[pos].flags & ZMAP_ENTRY_VALID)){
                                _data[pos].hash = olddata[i].hash;
                                // Move elements without copy constructors
                                _kalloc.rawmove(&olddata[i].key, &_data[pos].key);
                                _talloc.rawmove(&olddata[i].value, &_data[pos].value);
                                _data[pos].flags |= ZMAP_ENTRY_VALID;
                                break;
                            }
                        }
                        //throw ZException("Fatal Error in addHashEntry");
                    }
                }
                _alloc->dealloc(olddata);
//                olddata = nullptr;
            }
        }
    }

    // TODO: ZMap erase
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
    void setFactor(float factor){
        if(factor > 1.0f)
            throw ZException("Invalid ZMap load factor");
        _factor = factor;
    }

    ZMapIterator begin() const {
        return ZMapIterator(this, _head);
    }

    // For debugging
    MapElement &position(zu64 i){
        return _data[i];
    }

private:
    static zu64 _getHash(const K &key){
        return ZHash<K>(key).hash();
    }
    zu64 _getPos(zu64 hash, zu64 i) const {
        return ((hash % _realsize) + i) % _realsize;
    }

public:
    class ZMapIterator : public ZConstSimplexIterator<K> {
    public:
        ZMapIterator(const ZMap<K,T> *map, MapElement *start_elem) : _map(map), _elem(start_elem){}

        const K &get() const {
            return _elem->key;
        }

        bool more() const {
            return (_elem != nullptr);
        }
        void advance(){
            _elem = _elem->next;
        }

    private:
        const ZMap<K,T> *_map;
        ZMap<K,T>::MapElement *_elem;
    };

private:
    //! Memory allocator
    ZPointer<ZAllocator<MapElement>> _alloc;
    //! Used to construct and destroy keys
    ZAllocator<K> _kalloc;
    //! Used to construct and destroy values
    ZAllocator<T> _talloc;

    //! Actual data buffer.
    MapElement *_data;
    //! Pointer to first inserted element.
    MapElement *_head;
    //! Pointer to last inserted element.
    MapElement *_tail;
    //! Number of entries
    zu64 _size;
    //! Size of buffer
    zu64 _realsize;
    //! Max load factor (size / realsize ratio)
    float _factor;
};

}

//#undef ZMAP_ENTRTY_VALID
#undef ZMAP_ENTRTY_DELETED

#endif // ZMAP_H
