#include "zencrypt.h"

#include "hex.h"
#include "osrng.h"
#include "cryptlib.h"
#include "filters.h"
#include "aes.h"
#include "gcm.h"

using namespace CryptoPP;

namespace LibChaos {

ZEncrypt::ZEncrypt(){

}

ZEncrypt::~ZEncrypt(){

}

bool ZEncrypt::keyLoad(ZBinary key){

    return false;
}

bool ZEncrypt::encrypt(const ZBinary &plain, ZBinary &ciper){

    GCM<AES>::Encryption e;

    return false;
}

bool ZEncrypt::decrypt(const ZBinary &ciper, ZBinary &plain){

    GCM<AES>::Decryption d;

    return false;
}

}
