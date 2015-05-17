/*******************************************************************************
**                                  LibChaos                                  **
**                                zencrypt.cpp                                **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zencrypt.h"

//#include "cryptopp562/hex.h"
//#include "cryptopp562/osrng.h"
//#include "cryptopp562/cryptlib.h"
//#include "cryptopp562/filters.h"
//#include "cryptopp562/aes.h"
//#include "cryptopp562/gcm.h"

//using namespace CryptoPP;

namespace LibChaos {

ZEncrypt::ZEncrypt(){

}

ZEncrypt::~ZEncrypt(){

}

bool ZEncrypt::keyLoad(ZBinary key){

    return false;
}

bool ZEncrypt::encrypt(const ZBinary &plain, ZBinary &ciper){

    //GCM<AES>::Encryption e;

    return false;
}

bool ZEncrypt::decrypt(const ZBinary &ciper, ZBinary &plain){

    //GCM<AES>::Decryption d;

    return false;
}

}
