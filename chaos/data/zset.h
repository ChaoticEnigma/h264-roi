#ifndef ZSET_H
#define ZSET_H

#include "ztypes.h"
#include "zhash.h"
#include "zpointer.h"
#include "ziterator.h"

#define ZSET_DEFAULT_LOAD_FACTOR    0.5
#define ZSET_INITIAL_CAPACITY       16

namespace LibChaos {

template <typename T> class ZSet {
public:
    enum { none = ZU64_MAX };
public:
    typedef zu64 maphash;

    struct SetElement {
        zbyte flags;    // Some flags for unset and deleted
        zu64 hash;
        T value;
        //SetElement *next;
    };
public:
    ZSet(float loadfactor = ZSET_DEFAULT_LOAD_FACTOR, ZAllocator<SetElement> *alloc = new ZAllocator<SetElement>) :
            _alloc(alloc),_data(nullptr), _size(0), _realsize(0), _factor(loadfactor){
        resize(ZSET_INITIAL_CAPACITY);
    }

    ZSet(std::initializer_list<T> list) : ZSet(){
        resize(list.size());
        for(auto item = list.begin(); item < list.end(); ++item){
            add(item->key, item->value);
        }
    }

    ZSet(const ZSet &other) : _alloc(other._alloc), _kalloc(new ZAllocator<K>), _talloc(new ZAllocator<T>),
                              _data(nullptr), _size(0), _realsize(0), _factor(other._factor){
        if(other._data != nullptr && other._size > 0){
            resize(other._size);
            for(zu64 i = 0; i < other._size; ++i){
                if(other._data[i].flags & ZMAP_ENTRY_VALID){
                    add(other._data[i].key, other._data[i].value);
                }
            }
        }
    }

    ~ZMap(){
        if(_data != nullptr){
            for(zu64 i = 0; i < _realsize; ++i){
                if(_data[i].flags & ZMAP_ENTRY_VALID){
                    _kalloc->destroy(_kalloc->addressOf(_data[i].key));
                    _talloc->destroy(_talloc->addressOf(_data[i].value));
                }
            }
            _alloc->dealloc(_data);
//            _data = nullptr;
        }
        delete _kalloc;
        delete _talloc;
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
                _kalloc->construct(_kalloc->addressOf(_data[pos].key), key);
                _talloc->construct(_talloc->addressOf(_data[pos].value), value);
                _data[pos].flags |= ZMAP_ENTRY_VALID; // Set valid bit
                _data[pos].flags &= ~ZMAP_ENTRY_DELETED; // Unset deleted bit
                ++_size;
                return _data[pos].value;
            } else if(_data[pos].hash == hash){
                // Compare the actual key - may be non-trivial
                if(_data[pos].key == key){
                    // Reassign key and value in existing entry
                    _kalloc->destroy(_kalloc->addressOf(_data[pos].key));
                    _kalloc->construct(_kalloc->addressOf(_data[pos].key), key);
                    _talloc->destroy(_talloc->addressOf(_data[pos].value));
                    _talloc->construct(_talloc->addressOf(_data[pos].value), value);
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
                        _kalloc->destroy(_kalloc->addressOf(_data[pos].key));
                        _talloc->destroy(_talloc->addressOf(_data[pos].value));
                        _data[pos].flags &= ~ZMAP_ENTRY_VALID; // Unset valid bit
                        _data[pos].flags |= ZMAP_ENTRY_DELETED; // Set deleted bit
                        --_size;
                    }
                }
           } else {
                if(!(_data[pos].flags & ZMAP_ENTRY_DELETED)){
                    // If this is not a deleted entry, it is the end of the chain
                    break;
                }
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
            } else {
                if(!(_data[pos].flags & ZMAP_ENTRY_DELETED)){
                    // If this is not a deleted entry, it is the end of the chain
                    break;
                }
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

            SetElement *olddata = _data;
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
                                _kalloc->rawmove(_kalloc->addressOf(olddata[i].key), _kalloc->addressOf(_data[pos].key));
                                _talloc->rawmove(_talloc->addressOf(olddata[i].value), _talloc->addressOf(_data[pos].value));
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

    // For debugging
    SetElement &position(zu64 i){
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
    class ZSetIterator : public ZSimplexIterator<T> {
    public:
        ZSetIterator(ZSet<T> *set, typename ZSet<T>::SetElement *start_elem) : _set(set), _elem(start_elem){}

        T &get(){
            return _elem->value;
        }

        bool more() const {
            return (_prev == nullptr || _node != _list->_head);
        }
        void advance(){
            _prev = _node;
            _node = _node->next;
        }
    private:
        ZSet<T> *_set;
        ZSet<T>::SetElement *_elem;
    };

private:
    //! Memory allocator.
    ZPointer<ZAllocator<SetElement>> _alloc;
    //! Allocator only for constructing T inside nodes.
    ZAllocator<T> _talloc;

    //! Actual data buffer
    SetElement *_data;
    // Pointer to first inserted element
    //MapData *_head;
    // Pointer to last inserted element
    //MapData *_tail;
    //! Number of entries
    zu64 _size;
    //! Size of buffer
    zu64 _realsize;
    //! Max load factor (size / realsize ratio)
    float _factor;
};

}

#endif // ZSET_H
