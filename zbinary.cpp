#include "zbinary.h"

namespace LibChaos {

ZBinary::ZBinary() : _data(){}
ZBinary::ZBinary(char *ptr, zu64 len) : _data(ptr, ptr + len){}

//char &ZBinary::operator[](zu64 inx){
//    return _data[inx];
//}

zu64 ZBinary::size(){
    return _data.size();
}

std::vector<char> &ZBinary::data(){
    return _data;
}
char *ZBinary::raw(){
    return _data.data();
}

}
