/*******************************************************************************
**                                  LibChaos                                  **
**                                  zhash.cpp                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zhash.h"
#include "zexception.h"
#include "xxhash.h"
#include <functional>

namespace LibChaos {

zu64 ZHash64Base::simpleHash64_hash(const zbyte *data, zu64 size, zu64 seed){
    zu64 hash = seed ? seed : 5381;
    for(zu64 i = 0; i < size; ++i){
        hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
    }
    return hash;
}

zu64 ZHash64Base::xxHash64_hash(const zbyte *data, zu64 size){
    return XXH64(data, size, 0);
}
void *ZHash64Base::xxHash64_init(){
    return (void *)XXH64_createState();
}
void ZHash64Base::xxHash64_feed(void *state, const zbyte *data, zu64 size){
    XXH64_update(state, data, size);
}
zu64 ZHash64Base::xxHash64_done(void *state){
    zu64 hash = XXH64_digest(state);
    XXH64_freeState(state);
    return hash;
}

}
