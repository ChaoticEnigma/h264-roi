/*******************************************************************************
**                                  LibChaos                                  **
**                                  zlist.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZLIST_H
#define ZLIST_H

#include "ztypes.h"
#include "zallocator.h"
#include "ziterator.h"
#include "yindexedaccess.h"
#include "ypushpopaccess.h"

#include <initializer_list>

// For debug()
#include "zstring.h"

namespace LibChaos {

template <typename T> class ZList;
template <typename T> class ZIterator<ZList<T>>;

/*! Linked list sequence container.
 *  Implemented as a circular doubly-linked list.
 *  ZList push/pop paradigm is FIFO.
 */
template <typename T> class ZList : public YIndexedAccess<T>, public YPushPopAccess<T> {
    friend class ZIterator<ZList<T>>;
public:
    struct Node {
        Node *prev;
        Node *next;
        T data;
    };

public:
    ZList(ZAllocator<Node> *alloc = new ZAllocator<Node>()) : _alloc(alloc), _talloc(new ZAllocator<T>()), _size(0), _head(nullptr){}
    ZList(std::initializer_list<T> ls) : ZList(){
        for(auto item = ls.begin(); item < ls.end(); ++item)
            pushBack(*item);
    }

    ZList(const T *data, zu64 size) : ZList(){
        for(zu64 i = 0; i < size; ++i)
            pushBack(data[i]);
    }

    ~ZList(){
        if(_head != nullptr){
            _head->prev->next = nullptr; // Break circular link
            Node *current = _head;
            Node *next;
            do { // Delete all nodes
                _talloc->destroy(&(current->data));
//                _talloc->dealloc(&(current->data));
                next = current->next;
//                _alloc->destroy(current);
                _alloc->dealloc(current);
                current = next;
            } while(current != nullptr);
        }
        delete _talloc;
        delete _alloc;
    }

    //! Insert \a data before \a node.
    Node *insert(const T &data, Node *node){
        Node *newnode = newNode(data);
        newnode->next = node;
        newnode->prev = node->prev;
        node->prev->next = newnode;
        node->prev = newnode;
        return newnode;
    }

    //! Add \a data to the front of the list.
    void pushFront(const T &data){
        Node *node = newNode(data);
        if(_head == nullptr){
            _head = node;
            node->prev = _head;
            node->next = _head;
        } else {
            _head->prev->next = node;
            _head->next->prev = node;
            node->prev = _head->prev;
            node->next = _head;
            _head = node;
        }
        ++_size;
    }
    void pushBack(const T &data){
        Node *node = newNode(data);
        if(_head == nullptr){
            _head = node;
            node->prev = _head;
            node->next = _head;
        } else {
            node->prev = _head->prev;
            node->next = _head;
            _head->prev->next = node;
            _head->prev = node;
        }
        ++_size;
    }
    inline void push(const T &data){ pushBack(data); }

    void popFront(){
        if(_head != nullptr){
            Node *old = _head;
            if(_size > 1){
                _head->prev->next = _head->next;
                _head->next->prev = _head->prev;
                _head = _head->next;
            } else {
                _head = nullptr;
            }
            --_size;
            _talloc->destroy(&(old->data));
//            _talloc->dealloc(old->data);
//            _alloc->destroy(old);
            _alloc->dealloc(old);
        }
    }
    void popBack(){
        if(_head != nullptr){
            Node *old = _head->prev;
            if(_size > 1){
                _head->prev->prev->next = _head;
                _head->prev = _head->prev->prev;
            } else {
                _head = nullptr;
            }
            --_size;
            _talloc->destroy(&(old->data));
//            _talloc->dealloc(old->data);
//            _alloc->destroy(old);
            _alloc->dealloc(old);
        }
    }
    inline void pop(){ popFront(); }

    T &peekFront(){
        return _head->data;
    }
    const T &peekFront() const {
        return _head->data;
    }
    T &peekBack(){
        return _head->prev->data;
    }
    const T &peekBack() const {
        return _head->prev->data;
    }

    inline T &peek(){ return peekFront(); }
    inline const T &peek() const { return peekFront(); }

    // Swap data but NOT allocators of two lists
    void swap(ZList &other){
        zu64 tmpsize = _size;
        Node *tmphead = _head;
        _size = other._size;
        _head = other._head;
        other._size = tmpsize;
        other._head = tmphead;
    }

    //! Get an iterator for the list.
    ZIterator<ZList<T>> begin(){
        return ZIterator<ZList<T>>(this, _head);
    }
    ZIterator<ZList<T>> end(){
        return ZIterator<ZList<T>>(this, _head->prev);
    }

    ZString debug() const {
        ZString str;
        Node *current = _head;
        for(zu64 i = 0; i < _size; ++i){
            str += ZString::ItoS((zu64)current->prev, 16) + " ";
            str += ZString::ItoS((zu64)current, 16);
            str += " " + ZString::ItoS((zu64)current->next, 16);
            str += ", ";
            current = current->next;
        }
        return str;
    }

    inline T &operator[](zu64 index){ return getNode(index)->data; }
    inline const T &operator[](zu64 index) const { return getNode(index)->data; }

    inline T &front(){ return _head->data; }
    inline const T &front() const { return _head->data; }
    inline T &back(){ return _head->prev->data; }
    inline const T &back() const { return _head->prev->data; }

    inline bool isEmpty() const { return (_size == 0); }
    inline zu64 size() const { return _size; }

private:
    Node *newNode(const T &data){
       Node *node = _alloc->alloc();
//       _alloc->construct(node);
//       node->data = _talloc->alloc();
       node->prev = nullptr;
       node->next = nullptr;
       _talloc->construct(&(node->data), data);
       return node;
    }

    Node *getNode(zu64 index) const {
        Node *current = _head;
        for(zu64 i = 0; i < index; ++i){
            current = current->next;
        }
        return current;
    }

private:
    ZAllocator<Node> *_alloc;
    ZAllocator<T> *_talloc;
    zu64 _size;
    Node *_head;
};

template <typename T> class ZIterator<ZList<T>> : public ZIterable<T> {
public:
    ZIterator(ZList<T> *list, typename ZList<T>::Node *start_node) : _list(list), _node(start_node), _end(0){}

    T &get(){
        if(!_used){
            _used = true;
            advance();
        }
        return _node->data;
    }
    void advance(){
        if(_end == 2)
            _end = 0;
        if(_node == _list->_head->prev)
            _end = 1;
        _node = _node->next;
    }
    void recede(){
        if(_end == 1)
            _end = 0;
        if(_node == _list->_head)
            _end = 2;
        _node = _node->prev;
    }
    bool atEnd() const {
        return (_end != 0);
    }

    bool compare(ZIterator<ZList<T>> *it) const {
        return (_list == it->_list && _node == it->_node);
    }

    ZITERABLE_COMPARE_OVERLOADS(ZIterator<ZList<T>>)

private:
    ZList<T> *_list;
    typename ZList<T>::Node *_node;
    bool _used;
    char _end;
};

}

#endif // ZLIST_H
