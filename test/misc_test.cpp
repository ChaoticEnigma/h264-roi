#include "test.h"
#include "zuid.h"

int uid_block(){
    ZUID uid1;
    LOG(uid1.str() << " " << ZString::ItoS(uid1.getID().first, 16, 16) << " " << ZString::ItoS(uid1.getID().second, 16, 16));

    ZString uidstr2 = "abc569ff-1234-5678-abcd-defb56238ab4";
    ZUID uid2 = uidstr2;
    LOG(uidstr2);
    LOG(uid2.str() << " " << uid2.getID().first << " " << uid2.getID().second);

    ZString uidstr3 = "o23h4oi23h4";
    ZUID uid3 = uidstr3;
    LOG(uidstr3 << " " << uid3.str() << " " << uid3.getID().first << " " << uid3.getID().second);

    return 0;
}
