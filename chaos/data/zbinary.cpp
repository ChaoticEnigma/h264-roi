#include "zbinary.h"

namespace LibChaos {

ZBinary::ZBinary() : _data(){}
ZBinary::ZBinary(const void *ptr, zu64 len) : _data((const zbinary_type *)ptr, len){}

zu64 ZBinary::hash(char type, const ZBinary &data){
    switch(type){
    case 1: {
        zu64 hash = 5381;
        for(zu64 i = 0; i < data.size(); ++i){
            hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
        }
        return hash;
    }
    case 2:
        return 0;
    default:
        return 0;
    }
}

}
