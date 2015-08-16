/*******************************************************************************
**                                  LibChaos                                  **
**                                 zstorage.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZSTORAGE_H
#define ZSTORAGE_H

#include "ztypes.h"

namespace LibChaos {

/*! Defines an interface for c
 *  The ZStorage abstract class defines an interface
 */

// Implementations are expected to behave as follows.
// Implementations should expect the following behaviors of other implementations.
// Implementations should not expect any other behaviors from other implementations.

// 1. A ZStorage object *may* represent more data than can be stored in memory
// 2. Implementations should only call copy constructors of stored objects if the method requires it
// 2. getBlock() may allocate <size> bytes in order to map data to memory
// 3. freeBlock() and commitBlock() may invalidate a pointer returned from getBlock
// 4. getType() and setType() implicity call copy constructors for
// 5. this pointers for stored non-POD types should not be considered constant during the object's lifetime

class ZStorage {
public:
    virtual ~ZStorage(){}

    // Copy the contents of other byte-for-byte
    virtual void copy(const ZStorage *other) = 0;
    // Copy the contents of other as an array of T, calling copy constructors
    //template <typename T>
    //virtual void copyType(const ZStorage *other) = 0;

    virtual void copyToBlock(const zbyte *data, zu64 index, zu64 size) = 0;
    virtual void copyBlockTo(zu64 index, zu64 size, zbyte *data) const = 0;
    virtual ZStorage *newCopy() const = 0;

    // Get byte at index
    virtual zbyte get(zu64 index) const = 0;
    // Set byte at index
    virtual void set(zu64 index, zbyte byte) = 0;

    // Get T at index. Note index is the n-th T.
    //template <typename T>
    //virtual T getType(zu64 index) const = 0;
    // Set T at index. Note index is the n-th T.
    //template <typename T>
    //virtual void setType(zu64 index, T data) = 0;

    // Get pointer to block of size at index
    // NEVER store this pointer, always pass it to freeBlock when done
    virtual zbyte *getBlock(zu64 index, zu64 size) = 0;
    // Invalidate a pointer returned by getBlock
    virtual void freeBlock(zbyte *ptr) = 0;
    // Commit changes to a pointer returned by getBlock
    virtual void commitBlock(zbyte *ptr) = 0;

    virtual bool compare(const ZStorage *other) const = 0;

    // Set a new container size
    virtual void resize(zu64 size) = 0;

    // Set size to zero
    virtual void clear() = 0;

    // Size in bytes of contained data
    virtual zu64 size() const = 0;

    // Return short indicating type of container
    virtual zu16 storageType() const = 0;
    // Return string describing container
    virtual const char *storageTypeStr() const = 0;
};

}

#endif // ZSTORAGE_H
