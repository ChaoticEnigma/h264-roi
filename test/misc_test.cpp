#include "test.h"
#include "zuid.h"

int uid_block(){
    ZUID uid1;
    LOG(uid1.str());

    ZString uidstr2 = "abc569ff-1234-5678-abcd-defb56238ab4";
    ZUID uid2 = uidstr2;
    LOG(uidstr2);
    LOG(uid2.str());

    ZString uidstr3 = "o23h4oi23h4";
    ZUID uid3 = uidstr3;
    LOG(uidstr3);
    LOG(uid3.str());

    return 0;
}
