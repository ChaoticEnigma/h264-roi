/*******************************************************************************
**                                  LibChaos                                  **
**                                  zhash.cpp                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zhash.h"
#include "zexception.h"
#include <functional>
#include "xxhash.h"

//#include "fnv.h"

namespace LibChaos {

zu64 ZHash64Base::simpleHash64_hash(const zbyte *data, zu64 size, zu64 seed){
    zu64 hash = seed;
    for(zu64 i = 0; i < size; ++i){
        hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
    }
    return hash;
}


/*
 * fnv_64a_buf - perform a 64 bit Fowler/Noll/Vo FNV-1a hash on a buffer
 *
 * input:
 *	buf	- start of buffer to hash
 *	len	- length of buffer in octets
 *	hval	- previous hash value or 0 if first call
 *
 * returns:
 *	64 bit hash as a static hash type
 *
 * NOTE: To use the recommended 64 bit FNV-1a hash, use FNV1A_64_INIT as the
 * 	 hval arg on the first call to either fnv_64a_buf() or fnv_64a_str().
 */
//Fnv64_t fnv_64a_buf(const void *buf, size_t len, Fnv64_t hval){
//    const unsigned char *bp = (const unsigned char *)buf;   /* start of buffer */
//    const unsigned char *be = bp + len;                     /* beyond end of buffer */
//
//    // FNV-1a hash each octet of the buffer
//    while (bp < be) {
//        // xor the bottom with the current octet
//        hval ^= (Fnv64_t)*bp++;
//
//        // multiply by the 64 bit FNV magic prime mod 2^64
//        //hval *= FNV_64_PRIME;
//
//        // GGC OPTIMIZATIONs
//        hval += (hval << 1) + (hval << 4) + (hval << 5) + (hval << 7) + (hval << 8) + (hval << 40);
//    }
//    return hval;
//}

zu64 ZHash64Base::fnvHash64_hash(const zbyte *data, zu64 size, zu64 hval){
    // FNV-1a is recommended where possible by the developers
    //return fnv_64a_buf(data, size, hval);

    // The FNV-1a hash algorithm
    const zbyte *bp = data;         /* start of buffer */
    const zbyte *be = bp + size;    /* beyond end of buffer */

    // FNV-1a hash each octet of the buffer
    while (bp < be) {
        // xor the bottom with the current octet
        hval ^= (zu64)*bp++;

        // multiply by the 64 bit FNV magic prime mod 2^64
        //hval *= FNV_64_PRIME;

        // GGC OPTIMIZATIONs
        hval += (hval << 1) + (hval << 4) + (hval << 5) + (hval << 7) + (hval << 8) + (hval << 40);
    }
    return hval;
}

zu64 ZHash64Base::xxHash64_hash(const zbyte *data, zu64 size){
    return XXH64(data, size, 0);
}
void *ZHash64Base::xxHash64_init(){
    return (void *)XXH64_createState();
}
void ZHash64Base::xxHash64_feed(void *state, const zbyte *data, zu64 size){
    XXH64_update((XXH64_state_t*)state, data, size);
}
zu64 ZHash64Base::xxHash64_done(void *state){
    zu64 hash = XXH64_digest((const XXH64_state_t*)state);
    XXH64_freeState((XXH64_state_t*)state);
    return hash;
}

}
