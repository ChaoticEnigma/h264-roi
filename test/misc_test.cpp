#include "test.h"
#include "zuid.h"

int uid_block(){
    ZUID uid1;
    LOG(uid1.str());

    ZString uidstr2 = "abcdef00-1234-5678-9012-fedcbaabcdef";
    ZUID uid2 = uidstr2;
    LOG(uidstr2 << " " << uid2.str() << " " << (uid2 == ZUID_NIL));

    ZString uidstr3 = "o23h4oi23h4";
    ZUID uid3 = uidstr3;
    LOG(uidstr3 << " " << uid3.str() << " " << (uid3 == ZUID_NIL));

    return 0;
}
