#ifndef ZLIST_H
#define ZLIST_H

#include "ztypes.h"
#include "zallocator.h"
#include "yindexedaccess.h"

#include "zlog.h"

namespace LibChaos {

// Implemented as a circular doubly-linked list
template <typename T> class ZList : public YIndexedAccess<T> {
public:
    ZList() : _size(0), head(nullptr){}

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
            head = node;
            node->prev = head;
            node->next = head;
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
            head->prev->next = head->next;
            head->next->prev = head->prev;
            Node *old = head;
            head = head->next;
            --_size;
            _alloc.destroy(old->data);
            _alloc.dealloc(old->data);
            _nodealloc.destroy(old);
            _nodealloc.dealloc(old);
        }
    }
    void popBack(){
        if(head != nullptr){
            head->prev->prev->next = head;
            Node *old = head->prev;
            head->prev = head->prev->prev;
            --_size;
            _alloc.destroy(old->data);
            _alloc.dealloc(old->data);
            _nodealloc.destroy(old);
            _nodealloc.dealloc(old);
        }
    }
    inline void pop(){ popBack(); }

    void debug() const {
        ZString str;
        Node *current = head;
        for(int i = 0; i < _size; ++i){
            str += ZString::ItoS((zu64)current->prev, 16) + " ";
            str += ZString::ItoS((zu64)current, 16);
            str += " " + ZString::ItoS((zu64)current->next, 16);
            str += ", ";
            current = current->next;
        }
        LOG(str);
    }

    T &operator[](zu64 index){
        return *getNode(index)->data;
    }
    const T &operator[](zu64 index) const {
        return *getNode(index)->data;
    }

    T &front(){
        return *head->data;
    }
    const T &front() const {
        return *head->data;
    }
    T &back(){
        return *head->prev->data;
    }
    const T &back() const {
        return *head->prev->data;
    }

    zu64 size() const {
        return _size;
    }

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
