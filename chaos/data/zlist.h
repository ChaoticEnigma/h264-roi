/*******************************************************************************
**                                  LibChaos                                  **
**                                  zlist.h                                   **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZLIST_H
#define ZLIST_H

#include "ztypes.h"
#include "zallocator.h"

#include "yindexedaccess.h"
#include "ypushpopaccess.h"

#include "zstring.h"

namespace LibChaos {

// Implemented as a circular doubly-linked list
// ZList push/pop paradigm is FIFO
template <typename T> class ZList : public YIndexedAccess<T>, public YPushPopAccess<T> {
public:
    ZList() : _size(0), head(nullptr){}

    ZList(const T *data, zu64 size) : ZList(){
        for(zu64 i = 0; i < size; ++i)
            pushBack(data[i]);
    }

    ~ZList(){
        if(head != nullptr){
            head->prev->next = nullptr; // Break circular link
            Node *current = head;
            Node *next;
            do { // Delete all nodes
                _alloc.destroy(current->data);
                _alloc.dealloc(current->data);
                next = current->next;
                _nodealloc.destroy(current);
                _nodealloc.dealloc(current);
                current = next;
            } while(current != nullptr);
        }
    }

    void pushFront(const T &data){
        Node *node = newNode(data);
        if(head == nullptr){
            head = node;
            node->prev = head;
            node->next = head;
        } else {
            node->prev = head->prev;
            node->next = head;
            head = node;
            head->next->prev = head;
            head->prev->next = head;
        }
        ++_size;
    }
    void pushBack(const T &data){
        Node *node = newNode(data);
        if(head == nullptr){
            node->prev = node;
            node->next = node;
            head = node;
        } else {
            node->prev = head->prev;
            node->next = head;
            head->prev = node;
            head->prev->prev->next = node;
        }
        ++_size;
    }
    inline void push(const T &data){ pushBack(data); }

    void popFront(){
        if(head != nullptr){
            Node *old = head;
            if(_size > 1){
                head->prev->next = head->next;
                head->next->prev = head->prev;
                head = head->next;
            } else {
                head = nullptr;
            }
            --_size;
            _alloc.destroy(old->data);
            _alloc.dealloc(old->data);
            _nodealloc.destroy(old);
            _nodealloc.dealloc(old);
        }
    }
    void popBack(){
        if(head != nullptr){
            Node *old = head->prev;
            if(_size > 1){
                head->prev->prev->next = head;
                head->prev = head->prev->prev;
            } else {
                head = nullptr;
            }
            --_size;
            _alloc.destroy(old->data);
            _alloc.dealloc(old->data);
            _nodealloc.destroy(old);
            _nodealloc.dealloc(old);
        }
    }
    inline void pop(){ popFront(); }

    T &peekFront(){
        return *head->data;
    }
    const T &peekFront() const {
        return *head->data;
    }
    T &peekBack(){
        return *head->prev->data;
    }
    const T &peekBack() const {
        return *head->prev->data;
    }

    inline T &peek(){ return peekFront(); }
    inline const T &peek() const { return peekFront(); }

    ZString debug() const {
        ZString str;
        Node *current = head;
        for(int i = 0; i < _size; ++i){
            str += ZString::ItoS((zu64)current->prev, 16) + " ";
            str += ZString::ItoS((zu64)current, 16);
            str += " " + ZString::ItoS((zu64)current->next, 16);
            str += ", ";
            current = current->next;
        }
        return str;
    }

    // Swap data but NOT allocators of two lists
    void swap(ZList &other){
        zu64 tmpsize = _size;
        Node *tmphead = head;
        _size = other._size;
        head = other.head;
        other._size = tmpsize;
        other.head = tmphead;
    }

    inline T &operator[](zu64 index){ return *getNode(index)->data; }
    inline const T &operator[](zu64 index) const { return *getNode(index)->data; }

    inline T &front(){ return *head->data; }
    inline const T &front() const { return *head->data; }
    inline T &back(){ return *head->prev->data; }
    inline const T &back() const { return *head->prev->data; }

    inline bool isEmpty() const { return (_size == 0); }
    inline zu64 size() const { return _size; }

private:
    struct Node {
        Node *prev;
        Node *next;
        T *data;
    };

private:
    Node *newNode(const T &data){
       Node *node = _nodealloc.alloc(1);
       _nodealloc.construct(node);
       node->data = _alloc.alloc(1);
       _alloc.construct(node->data, 1, data);
       return node;
    }

    Node *getNode(zu64 index) const {
        Node *current = head;
        for(zu64 i = 0; i < index; ++i){
            current = current->next;
        }
        return current;
    }

private:
    ZAllocator<T> _alloc;
    ZAllocator<Node> _nodealloc;
    zu64 _size;
    Node *head;
};

}

#endif // ZLIST_H
