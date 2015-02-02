#include "zrandom.h"

#include <assert.h>

namespace LibChaos {

ZRandom::ZRandom(){
    BOOL ret = CryptAcquireContext(&_cryptprov, ZString("ZUID_UUID_GENERATOR").wstr(), NULL, PROV_RSA_FULL, 0);
    assert(ret); // if this fails, burn it to the ground

}

LibChaos::ZBinary LibChaos::ZRandom::generate(LibChaos::zu64 size){

}

}
