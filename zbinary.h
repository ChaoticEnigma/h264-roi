#ifndef ZBINARY_H
#define ZBINARY_H

//#include <vector>

#include "ztypes.h"
#include "zarray.h"

namespace LibChaos {

class ZBinary {
public:
    ZBinary();
    ZBinary(const char *ptr, zu64 len);

//    ZBinary &operator=(ZBinary arr){
//        _data = arr.data();
//        return *this;
//    }

    inline char &operator[](zu64 inx){
        return _data[inx];
    }

    zu64 size() const;

    ZArray<char> &data();
    const char *raw() const;
private:
    ZArray<char> _data;
};

//typedef std::vector<char> ZBinary;

}

#endif // ZBINARY_H
