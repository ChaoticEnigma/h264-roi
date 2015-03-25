#include "test.h"
#include "zuid.h"
#include "zlist.h"
#include "zhash.h"

int uid_block(){
    ZUID uid1;
    LOG("Default (time):\t" << uid1.str());

    ZString uidstr2 = "abcdef00-1234-5678-9012-fedcbaabcdef";
    ZUID uid2 = uidstr2;
    LOG("String:\t\t" << uid2.str() << " " << (uid2.str() == uidstr2));

    ZString uidstr3 = "o23h4oi23h4";
    ZUID uid3 = uidstr3;
    LOG("String Fail:\t\t" << uid3.str() << " " << uidstr3 << " " << (uid3 == ZUID_NIL));

    ZUID uid4(ZUID::nil);
    LOG("Nil:\t\t" << uid4.str());

    ZUID uid5(ZUID::time);
    LOG("Time:\t\t" << uid5.str());

    ZUID uid6(ZUID::random);
    LOG("Random:\t\t" << uid6.str());

    ZArray<ZUID> uidlist;
    for(zu64 i = 0; i < 20; ++i){
        uidlist.push(ZUID());
    }
    for(zu64 i = 0; i < uidlist.size(); ++i){
        LOG(uidlist[i].str());
    }

    return 0;
}

int hash_block(){
    int data1 = -34563;
    zu64 hash1 = ZHash<int>(data1).hash();
    LOG(data1 << " " << hash1);

    ZString data2 = "hashdata";
    zu64 hash2 = ZHash<ZString>(data2).hash();
    LOG(data2 << " " << hash2);

    return 0;
}
