#ifndef ZSET_H
#define ZSET_H

#include "ztypes.h"
#include "zhash.h"
#include "zpointer.h"
#include "ziterator.h"

#define ZSET_DEFAULT_LOAD_FACTOR  0.5
#define ZSET_INITIAL_CAPACITY     16
#define ZSET_ENTRY_VALID          0x01 // valid data in entry
#define ZSET_ENTRY_DELETED        0x02 // previously deleted entry

namespace LibChaos {

template <typename T> class ZSet {
public:
    enum { none = ZU64_MAX };

    struct SetElement {
        zbyte flags; // Some flags for unset and deleted
        SetElement *next;
        SetElement *prev;
        zu64 hash;
        T value;
    };

    typedef zu64 maphash;

    class ZSetIterator;

public:
    ZSet(float loadfactor = ZSET_DEFAULT_LOAD_FACTOR, ZAllocator<SetElement> *alloc = new ZAllocator<SetElement>) :
            _alloc(alloc), _data(nullptr), _head(nullptr), _tail(nullptr), _size(0), _realsize(0), _factor(loadfactor){
        resize(ZSET_INITIAL_CAPACITY);
    }

    ZSet(std::initializer_list<T> list) : ZSet(){
        resize(list.size());
        for(auto item = list.begin(); item < list.end(); ++item){
            add(item);
        }
    }

    ZSet(const ZSet &other) : _alloc(other._alloc), _data(nullptr), _head(nullptr), _tail(nullptr),
            _size(0), _realsize(0), _factor(other._factor){
        if(other._data != nullptr && other._size > 0){
            resize(other._size);
            for(auto it = other.begin(); it.more(); ++it){
                add(*it);
            }
        }
    }

    ~ZSet(){
        SetElement *current = _head;
        while(current != nullptr){
            _talloc.destroy(&current->value);
            current = current->next;
        }
        _alloc->dealloc(_data);
    }

    //! Add \a value to set. Does nothing if set contains \a value.
    void add(const T &value){
        // Ensure enough space
        resize(_size + 1);

        zu64 hash = _getHash(value);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
            if(!(_data[pos].flags & ZSET_ENTRY_VALID)){
                // Entry is unset or deleted, insert new entry
                _data[pos].hash = hash;
                _talloc.construct(&_data[pos].value, value);
                _data[pos].flags |= ZSET_ENTRY_VALID; // Set valid bit
                _data[pos].flags &= ~ZSET_ENTRY_DELETED; // Unset deleted bit
                _data[pos].prev = _tail;
                _data[pos].next = nullptr;
                if(!_head) _head = _data + pos; // Set head if unset
                if(_tail) _tail->next = _data + pos; // Point prev element to this element
                _tail = _data + pos; // Update tail
                ++_size;
                return;
            } else if(_data[pos].hash == hash){
                // Compare the actual value - may be non-trivial
                if(_data[pos].value == value){
                    // Value already in set
                    return;
                }
            }
        }
        // Should only ever fail if resize failed and the set is full
        throw ZException(ZString("ZSet: Unable to add value to set: ") + hash);
    }
    inline T &push(const T &value){ return add(value); }

    //! Remove \a value from set. Does nothing if set does not contain \a value.
    void remove(const T &value){
        zu64 hash = _getHash(value);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
            if(_data[pos].flags & ZSET_ENTRY_VALID){
                // Valid data, compare hash
                if(_data[pos].hash == hash){
                    // Compare the actual value - may be non-trivial
                    if(_data[pos].value == value){
                        // Found value, delete element
                        _talloc.destroy(&_data[pos].value);
                        _data[pos].flags &= ~ZSET_ENTRY_VALID; // Unset valid bit
                        _data[pos].flags |= ZSET_ENTRY_DELETED; // Set deleted bit
                        if(_data[pos].prev) _data[pos].prev->next = _data[pos].next; // Point prev element to next
                        if(_data[pos].next) _data[pos].next->prev = _data[pos].prev; // Point next element to prev
                        if(_head == _data + pos) _head = _data[pos].next; // Move head if necessary
                        if(_tail == _data + pos) _tail = _data[pos].prev; // Move tail if necessary
                        --_size;
                        break;
                    }
                }
            } else if(!(_data[pos].flags & ZSET_ENTRY_DELETED)){
                // If this is not a deleted entry, it is the end of the chain
                break;
            }
        }
    }

    //! Check if set contains \a value.
    bool contains(const T &value) const {
        zu64 hash = _getHash(value);
        for(zu64 i = 0; i < _realsize; ++i){
            zu64 pos = _getPos(hash, i);
            if(_data[pos].flags & ZSET_ENTRY_VALID){
                // Valid data, compare hash
                if(_data[pos].hash == hash){
                    // Compare the actual value - may be non-trivial
                    if(_data[pos].value == value){
                        // Found value
                        return true;
                    }
                }
            } else if(!(_data[pos].flags & ZSET_ENTRY_DELETED)){
                // If this is not a deleted entry, it is the end of the chain
                break;
            }
        }
        // Did not find value
        return false;
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
                    if(olddata[i].flags & ZSET_ENTRY_VALID){
                        for(zu64 j = 0; j < _realsize; ++j){
                            zu64 pos = _getPos(olddata[i].hash, j);
                            // Find first empty entry
                            if(!(_data[pos].flags & ZSET_ENTRY_VALID)){
                                _data[pos].hash = olddata[i].hash;
                                // Move elements without copy constructors
                                _talloc.rawmove(&olddata[i].value, &_data[pos].value);
                                _data[pos].flags |= ZSET_ENTRY_VALID;
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

    bool isEmpty() const {
        return (_data == nullptr) || _size == 0;
    }

    zu64 size() const { return _size; }
    zu64 realSize() const { return _realsize; }

    float factor() const { return _factor; }
    void setFactor(float factor){
        if(factor > 1.0f)
            throw ZException("Invalid ZSet load factor");
        _factor = factor;
    }

    ZSetIterator begin() const {
        return ZSetIterator(this, _head);
    }

    // For debugging
    SetElement &position(zu64 i){
        return _data[i];
    }

private:
    static zu64 _getHash(const T &key){
        return ZHash<T>(key).hash();
    }
    zu64 _getPos(zu64 hash, zu64 i) const {
        return ((hash % _realsize) + i) % _realsize;
    }

public:
    class ZSetIterator : public ZConstSimplexIterator<T> {
    public:
        ZSetIterator(const ZSet<T> *set, ZSet<T>::SetElement *start_elem) : _set(set), _elem(start_elem){}

        const T &get() const {
            return _elem->value;
        }

        bool more() const {
            return (_elem != nullptr);
        }
        void advance(){
            _elem = _elem->next;
        }

    private:
        const ZSet<T> *_set;
        ZSet<T>::SetElement *_elem;
    };

private:
    //! Memory allocator.
    ZPointer<ZAllocator<SetElement>> _alloc;
    //! Allocator only for constructing T inside nodes.
    ZAllocator<T> _talloc;

    //! Actual data buffer.
    SetElement *_data;
    //! Pointer to first inserted element.
    SetElement *_head;
    //! Pointer to last inserted element.
    SetElement *_tail;
    //! Number of entries.
    zu64 _size;
    //! Size of buffer.
    zu64 _realsize;
    //! Max load factor (size / realsize ratio).
    float _factor;
};

}

#endif // ZSET_H
