#ifndef ZSTORAGE_H
#define ZSTORAGE_H

#include "ztypes.h"

namespace LibChaos {

class ZStorage {
public:
    virtual ~ZStorage(){}

    virtual void copy(const ZStorage *other) = 0;
    virtual void copyFrom(const zbyte *data, zu64 size) = 0;
    virtual void copyBlockTo(zu64 index, zu64 size, zbyte *data) const = 0;
    virtual ZStorage *newCopy() const = 0;

    // Get reference to byte at index
    virtual zbyte &get(zu64 index) = 0;
    virtual const zbyte &get(zu64 index) const = 0;

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
