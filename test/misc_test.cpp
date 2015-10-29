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
    for(zu64 i = 0 ; i < mac.size(); ++i)
        macstr += ZString::ItoS(mac[i], 16, 2) += ":";
    macstr.substr(0, macstr.size()-1);
    LOG(ZString("MAC:").pad(' ', PADLEN) << macstr << " " << mac.size() << " " << mac);

    ZList<ZBinary> maclist = ZUID::getMACAddresses();

    return 0;
}
