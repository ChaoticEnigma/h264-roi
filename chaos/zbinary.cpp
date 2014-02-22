#include "zbinary.h"

namespace LibChaos {

ZBinary::ZBinary() : _data(){}
ZBinary::ZBinary(const unsigned char *ptr, zu64 len) : _data(ptr, len){}

zu64 ZBinary::size() const{
    return _data.size();
}

ZArray<unsigned char> &ZBinary::data(){
    return _data;
}
const unsigned char *ZBinary::raw() const {
    return _data.ptr();
}

}
