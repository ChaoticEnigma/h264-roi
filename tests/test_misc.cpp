#include "tests.h"
#include "zrandom.h"
#include "zuid.h"
#include "zlist.h"
#include "zhash.h"

#define PADLEN 16
#define PAD(X) ZString(X).pad(' ', PADLEN)

namespace LibChaosTest {

void random(){
    ZRandom random;
    LOG(random.genzu());
}

void uid(){
    ZUID uid1;

    ZString uidstr2 = "abcdef00-1234-5678-9012-fedcbaabcdef";
    ZUID uid2 = uidstr2;
    TASSERT(uid2.str() == uidstr2);

    ZString uidstr3 = "abcdef0x-1234-5678-9012-fedcbaabcdef";
    ZUID uid3 = uidstr3;
    LOG(uid3.str());
    TASSERT(uid3 == ZUID_NIL);

    ZUID uid4(ZUID::NIL);
    ZUID uid5(ZUID::TIME);
    ZUID uid6(ZUID::RANDOM);

    ZUID ndns("6ba7b810-9dad-11d1-80b4-00c04fd430c8");
    ZString name = "www.znnxs.com";
    ZUID uid7(ZUID::NAME_MD5, ndns, name);
    ZUID uid8(ZUID::NAME_SHA, ndns, name);

    TASSERT(ZUID(ZUID::NAME_MD5, ndns, "www.widgets.com").str() == "e902893a-9d22-3c7e-a7b8-d6e313b71d9f");

    LOG(PAD("Default (nil):") << uid1.str());
    LOG(PAD("String:")         << uid2.str());
    LOG(PAD("String (fail):")  << uid3.str() << " " << uidstr3);
    LOG(PAD("Nil:")            << uid4.str());
    LOG(PAD("Time:")           << uid5.str());
    LOG(PAD("Random:")         << uid6.str());
    LOG(PAD("Name MD5:")       << uid7.str());
    LOG(PAD("Name SHA1:")      << uid8.str());

    ZBinary mac = ZUID::getMACAddress();
    ZString macstr;
    for(zu64 i = 0 ; i < mac.size(); ++i)
        macstr += ZString::ItoS(mac[i], 16, 2) += ":";
    macstr.substr(0, macstr.size()-1);
    LOG(ZString("MAC:").pad(' ', PADLEN) << macstr << " " << mac.size() << " " << mac);

    ZList<ZBinary> maclist = ZUID::getMACAddresses();
}

ZArray<Test> misc_tests(){
    return {
        { "random", random, true, {} },
        { "uid",    uid,    true, {} },
    };
}

}
