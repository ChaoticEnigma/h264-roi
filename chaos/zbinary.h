#ifndef ZBINARY_H
#define ZBINARY_H

//#include <vector>

#include "ztypes.h"
#include "zarray.h"

namespace LibChaos {

class ZBinary {
public:
    ZBinary();
    ZBinary(const unsigned char *ptr, zu64 len);

    inline unsigned char &operator[](zu64 inx){
        return _data[inx];
    }

    zu64 size() const;

    ZArray<unsigned char> &data();
    const unsigned char *raw() const;
private:
    ZArray<unsigned char> _data;
};

}

#endif // ZBINARY_H
