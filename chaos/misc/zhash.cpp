#include "zhash.h"
#include "zexception.h"
#include "xxhash.h"

namespace LibChaos {

zu64 ZHashBase::hashData(const zbyte *data, zu64 size, hashMethod method){
    switch(method){
        case hashType1: {
            zu64 hash = 5381;
            for(zu64 i = 0; i < size; ++i){
                hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
            }
            return hash;
        }
        default:
            throw ZException("Unknown hashMethod in hashData");
    }
}

}
