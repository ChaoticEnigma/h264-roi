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

typedef LibChaos::zu32 crc;

#define POLYNOMIAL          0x04C11DB7
#define INITIAL_REMAINDER   0xFFFFFFFF
#define FINAL_XOR_VALUE     0xFFFFFFFF
//#define CHECK_VALUE         0xCBF43926

#define WIDTH   32
#define TOPBIT  (1 << (WIDTH - 1))

#define REFLECT_DATA(X)         ((unsigned char)reflect((X), 8))
#define REFLECT_REMAINDER(X)    ((crc)reflect((X), WIDTH))

namespace LibChaos {

static zu32 reflect(zu32 data, unsigned char bits){
    zu32 reflection = 0x00000000;
    unsigned char bit;

    // * Reflect the data about the center bit.
    for(bit = 0; bit < bits; ++bit){
        // * If the LSB bit is set, set the reflection of it.
        if(data & 0x01){
            reflection |= (1 << (bits - bit - 1));
        }
        data = (data >> 1);
    }
    return reflection;
}

zu32 ZHash32Base::crcHash32_hash(const zbyte *data, zu64 size, zu32 remainder){
    remainder = (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);
    zu64 byte;
    unsigned char bit;

    // * Perform modulo-2 division, a byte at a time.
    for(byte = 0; byte < size; ++byte){
        // * Bring the next byte into the remainder.
        remainder ^= (REFLECT_DATA(data[byte]) << (WIDTH - 8));
        // Perform modulo-2 division, a bit at a time.
        for(bit = 8; bit > 0; --bit){
            // * Try to divide the current data bit.
            if(remainder & TOPBIT){
                remainder = (remainder << 1) ^ POLYNOMIAL;
            } else {
                remainder = (remainder << 1);
            }
        }
    }

    // * The final remainder is the CRC result.
    return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);
}

zu64 ZHash64Base::simpleHash64_hash(const zbyte *data, zu64 size, zu64 seed){
    zu64 hash = seed;
    for(zu64 i = 0; i < size; ++i){
        hash = ((hash << 5) + hash) + data[i]; /* hash * 33 + c */
    }
    return hash;
}

zu64 ZHash64Base::fnvHash64_hash(const zbyte *data, zu64 size, zu64 hval){
    // FNV (Fowler/Noll/Vo) hash algorithm
    // The FNV hash algorithm is in the public domain
    // FNV-1a is recommended where possible by the developers

    // The FNV-1a hash algorithm
    const zbyte *bp = data;         /* start of buffer */
    const zbyte *be = bp + size;    /* beyond end of buffer */

    // FNV-1a hash each octet of the buffer
    while (bp < be) {
        // xor the bottom with the current octet
        hval ^= (zu64)*bp++;

        // multiply by the 64 bit FNV magic prime mod 2^64
        //hval *= FNV_64_PRIME;
        // OR
        // GGC OPTIMIZATIONs
        hval += (hval << 1) + (hval << 4) + (hval << 5) + (hval << 7) + (hval << 8) + (hval << 40);
    }
    return hval;

    //return fnv_64a_buf(data, size, hval);
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
