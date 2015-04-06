#include "zhash.h"
#include "zexception.h"
#include "xxhash.h"
#include <functional>

namespace LibChaos {

zu64 ZHash64Base::simpleHash64_hash(const zbyte *data, zu64 size){
    return XXH64(data, size, 0);
}

zu64 ZHash64Base::xxHash64_hash(const zbyte *data, zu64 size){
    zu64 hash = 5381;
    for(zu64 i = 0; i < size; ++i){
        hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
    }
    return hash;
}

}
