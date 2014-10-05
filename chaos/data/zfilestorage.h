#ifndef ZFILESTORAGE_H
#define ZFILESTORAGE_H

#include "zstorage.h"

namespace LibChaos {

class ZFileStorage : public ZStorage {
public:
    static const zu16 type = 2;

public:
    ZFileStorage(){

    }

    ~ZFileStorage(){

    }

    void copy(const ZStorage *other){

    }
    void copyFrom(const zbyte *data, zu64 size){

    }
    void copyBlockTo(zu64 index, zu64 size, zbyte *data) const {

    }
    ZStorage *newCopy() const {

    }

    zbyte &get(zu64 index){

    }
    const zbyte &get(zu64 index) const {

    }

    bool compare(const ZStorage *other) const {

    }

    void resize(zu64 size){

    }
    void clear(){

    }

    zu64 size() const {

    }

    zu16 storageType() const {
        return type;
    }
    const char *storageTypeStr() const {
        return "ZFileStorage";
    }
};

}

#endif // ZFILESTORAGE_H
