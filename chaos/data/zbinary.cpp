#include "zbinary.h"

namespace LibChaos {

zu64 ZBinary::hash(ZBinary::HashType type, const ZBinary &data){
    switch(type){
        case hashType1: {
            zu64 hash = 5381;
            for(zu64 i = 0; i < data.size(); ++i){
                hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
            }
            return hash;
        }
        default:
            return 0;
    }
}

}
