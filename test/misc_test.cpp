#include "test.h"
#include "zrandom.h"
#include "zuid.h"
#include "zlist.h"
#include "zhash.h"

int random_test(){
    ZRandom random;
    LOG(random.genzu());

    return 0;
}

#define PADLEN 16
#define PAD(X) ZString(X).pad(' ', PADLEN)

int uid_test(){
    ZUID uid1;

    ZString uidstr2 = "abcdef00-1234-5678-9012-fedcbaabcdef";
    ZUID uid2 = uidstr2;
    TASSERT(uid2.str() == uidstr2);

    ZString uidstr3 = "abcdef0x-1234-5678-9012-fedcbaabcdef";
    ZUID uid3 = uidstr3;
    TASSERT(uid3 == ZUID_NIL);

    ZUID uid4(ZUID::NIL);
    ZUID uid5(ZUID::TIME);
    ZUID uid6(ZUID::RANDOM);

    LOG(PAD("Default (time):") << uid1.str());
    LOG(PAD("String:")         << uid2.str());
    LOG(PAD("String (fail):")  << uid3.str() << " " << uidstr3);
    LOG(PAD("Nil:")            << uid4.str());
    LOG(PAD("Time:")           << uid5.str());
    LOG(PAD("Random:")         << uid6.str());

    ZBinary mac = ZUID::getMACAddress();
    ZString macstr;
    for(zu64 i = 0 ; i < mac.size(); ++i){
        macstr += ZString::ItoS(mac[i], 16, 2) += ":";
    }
    macstr.substr(0, macstr.size()-1);
    LOG(ZString("MAC:").pad(' ', PADLEN) << macstr << " " << mac.size() << " " << mac);

    ZList<ZBinary> maclist = ZUID::getMACAddresses();

    return 0;
}

int hash_test(){
    int data1 = -34563;
    zu64 hash1 = ZHash<int>(data1).hash();
    LOG(data1 << " " << hash1);

    auto hf = [](ZString data){
        zu64 hash1a = ZHash<ZString>(data).hash();
        zu64 hash1b = ZHash<ZString>(data).hash();
        LOG("Default: " << data << " " << hash1a << " " << hash1b);
        TASSERT(hash1a == hash1b);

        zu64 hash2a = ZHash<ZString, ZHashBase::xxHash64>(data).hash();
        zu64 hash2b = ZHash<ZString, ZHashBase::xxHash64>(data).hash();
        LOG("XXHash: " << data << " " << hash2a << " " << hash2b);
        TASSERT(hash2a == hash2b);

        zu64 hash3a = ZHash<ZString, ZHashBase::fnvHash64>(data).hash();
        zu64 hash3b = ZHash<ZString, ZHashBase::fnvHash64>(data).hash();
        LOG("FNVHash: " << data << " " << hash3a << " " << hash3b);
        TASSERT(hash3a == hash3b)
    };


    ZString data2 = "hashdata";
    ZString data3 = "hashdata1";
    ZString data4 = "hashdata2";

    hf(data2);
    hf(data3);
    hf(data4);

    return 0;
}
