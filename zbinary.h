#ifndef ZBINARY_H
#define ZBINARY_H

#include <vector>

#include "ztypes.h"

namespace LibChaos {

class ZBinary {
public:
    ZBinary();
    ZBinary(char *ptr, zu64 len);

    inline char &operator[](zu64 inx){
        return _data[inx];
    }

    zu64 size();

    std::vector<char> &data();
    char *raw();
private:
    std::vector<char> _data;
};

//typedef std::vector<char> ZBinary;

}

#endif // ZBINARY_H
