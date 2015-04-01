#include "zrandom.h"
#include "zstring.h"
#include "zerror.h"
#include "zlog.h"

#include <assert.h>

namespace LibChaos {

ZRandom::ZRandom(){
#if PLATFORM == WINDOWS
    ZString keyset = "ZRANDOM_GENERATOR";
    // Try to use existing named keyset
    BOOL ret = CryptAcquireContextA(&_cryptprov, keyset.cc(), NULL, PROV_RSA_FULL, 0);
    if(ret == FALSE){
        if(GetLastError() == 0x80090016){
            // Create new named keyset
            ret = CryptAcquireContextA(&_cryptprov, keyset.cc(), NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET);
            if(ret == FALSE)
                ELOG(ZError::getSystemError());
        }
        ELOG(ZError::getSystemError());
    }
#elif PLATFORM == MACOSX
    // OSX's /dev/random is equivalent to UNIX's /dev/urandom
    _devrandom = fopen("/dev/random", "r");
    if(_devrandom == NULL)
        ELOG("Failed to open /dev/random:" << ZError::getSystemError());
#else
    // Use /dev/urandom for simplicity
    _devrandom = fopen("/dev/urandom", "r");
    if(_devrandom == NULL)
        ELOG("Failed to open /dev/urandom:" << ZError::getSystemError());
#endif
}

ZRandom::~ZRandom(){
#if PLATFORM == WINDOWS

#else
    fclose(_devrandom);
    _devrandom = NULL;
#endif
}

zu64 ZRandom::genzu(zu64 min, zu64 max){
    ZBinary buff = generate(8);
    zu64 rand = (zu64)buff[0] | (zu64)buff[1] << 8 | (zu64)buff[2] << 16 | (zu64)buff[3] << 24 | (zu64)buff[4] << 32 | (zu64)buff[5] << 40 | (zu64)buff[6] << 48 | (zu64)buff[7] << 56;
    rand = rand % (max - min) + min; // TODO: imperfect distribution
    return rand;
}

ZBinary ZRandom::generate(zu64 size){
    ZBuffer buffer(size);
#if PLATFORM == WINDOWS
    BOOL ret = CryptGenRandom(_cryptprov, size, buffer.raw());
    if(ret == FALSE)
        ELOG(ZError::getSystemError());
#else
    if(_devrandom == NULL)
        throw ZException("Random device is not open");
    else
        fread(buffer.raw(), 1, size, _devrandom);
#endif
    return buffer;
}

}
