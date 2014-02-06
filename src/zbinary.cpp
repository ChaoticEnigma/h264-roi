#include "zbinary.h"

namespace LibChaos {

ZBinary::ZBinary() : _data(){}
ZBinary::ZBinary(const char *ptr, zu64 len) : _data(ptr, len){}

//char &ZBinary::operator[](zu64 inx){
//    return _data[inx];
//}

zu64 ZBinary::size() const{
    return _data.size();
}

ZArray<char> &ZBinary::data(){
    return _data;
}
const char *ZBinary::raw() const {
    return _data.ptr();
}

}
