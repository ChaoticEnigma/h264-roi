#include "zrandom.h"
#include "zstring.h"
#include "zerror.h"
#include "zlog.h"

#include <assert.h>

namespace LibChaos {

ZRandom::ZRandom(){
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
}

ZBinary ZRandom::generate(zu64 size){
    ZBuffer buffer(size);
    BOOL ret = CryptGenRandom(_cryptprov, size, buffer.raw());
    if(ret == FALSE)
        ELOG(ZError::getSystemError());
    return buffer;
}

}
