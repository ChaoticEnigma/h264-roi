#ifndef ZREADER_H
#define ZREADER_H

#include "ztypes.h"

namespace LibChaos {

class ZReader {
public:
    ZReader() : _readerpos(0){}

    //virtual ~ZReader(){}

    virtual zu64 read(unsigned char *dest, zu64 length) = 0;
    virtual bool atEnd() const = 0;

    virtual void rewind(){
        _readerpos = 0;
    }

    virtual zu64 readPos() const {
        return _readerpos;
    }

protected:
    zu64 _readerpos;
};

}

#endif // ZREADER_H
