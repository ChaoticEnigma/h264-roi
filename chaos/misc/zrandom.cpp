#include "zrandom.h"
#include "zstring.h"
#include "zerror.h"
#include "zlog.h"

#include <assert.h>

namespace LibChaos {

ZRandom::ZRandom(){
#if PLATFORM == WINDOWS
    ZString keyset = "ZUID_UUID_GENERATOR";
    BOOL ret = CryptAcquireContextA(&_cryptprov, keyset.cc(), NULL, PROV_RSA_FULL, 0);
    if(ret == FALSE){
        if(GetLastError() == 0x80090016){
            ret = CryptAcquireContextA(&_cryptprov, keyset.cc(), NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET);
            if(ret == FALSE)
                ELOG(ZError::getSystemError());
        }
        ELOG(ZError::getSystemError());
    }
#elif PLATFORM == MACOSX
    _devrandom = fopen("/dev/random", "r");
    if(_devrandom == NULL)
        ELOG("Failed to open /dev/random:" << ZError::getSystemError());
#else
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
    zu64 rand = buff[0] | buff[1] << 1 | buff[2] << 2 | buff[3] << 3 | buff[4] << 4 | buff[5] << 5 | buff[6] << 6 | buff[7] << 7;
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
        ELOG("random device is not open");
    else
        fread(buffer.raw(), 1, size, _devrandom);
#endif
    return buffer;
}

}
