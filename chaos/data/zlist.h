#ifndef ZLIST_H
#define ZLIST_H

#include "ztypes.h"
#include "yindexedaccess.h"

namespace LibChaos {

template <typename T> class ZList : public YIndexedAccess<T> {
private:
    struct Node {
        Node *prev;
        Node *next;
        T data;
    };

public:
    ZList() : _size(0), head(nullptr){}

    void pushFront(const T &data){
        Node *node = new Node;
        node->data = data;
        if(head == nullptr){
            head = node;
            node->prev = head;
            node->next = head;
        } else {
            node->prev = head->prev;
            node->next = head;
            head = node;
        }
        ++_size;
    }
    void pushBack(const T &data){
        Node *node = new Node;
        node->data = data;
        if(head == nullptr){
            head = node;
            node->prev = head;
            node->next = head;
        } else {
            node->prev = head->prev;
            node->next = head;
            head->prev = node;
        }
        ++_size;
    }
    inline void push(const T &data){ pushBack(data); }

    T &operator[](zu64 index){
        return getNode(index)->data;
    }
    const T &operator[](zu64 index) const {
        return getNode(index)->data;
    }

    zu64 size() const {
        return _size;
    }

private:
    Node *getNode(zu64 index) const {
        Node *current = head;
        for(zu64 i = 0; i < index; ++i){
            current = current->next;
        }
        return current;
    }

private:
    zu64 _size;
    Node *head;
};

}

#endif // ZLIST_H
