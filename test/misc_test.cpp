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

int uid_test(){
    ZUID uid1;
    LOG("Default (time):\t" << uid1.str());

    ZString uidstr2 = "abcdef00-1234-5678-9012-fedcbaabcdef";
    ZUID uid2 = uidstr2;
    LOG("String:\t" << uid2.str() << " " << (uid2.str() == uidstr2));

    ZString uidstr3 = "o23h4oi23h4";
    ZUID uid3 = uidstr3;
    LOG("String Fail:\t" << uid3.str() << " " << uidstr3 << " " << (uid3 == ZUID_NIL));

    ZUID uid4(ZUID::NIL);
    LOG("Nil:\t\t" << uid4.str());

    ZUID uid5(ZUID::TIME);
    LOG("Time:\t\t" << uid5.str());

    ZUID uid6(ZUID::RANDOM);
    LOG("Random:\t" << uid6.str());

    ZBinary mac = ZUID::getMACAddress();
    ZString macstr;
    for(zu64 i = 0 ; i < mac.size(); ++i){
        macstr += ZString::ItoS(mac[i], 16, 2) += ":";
    }
    macstr.substr(0, macstr.size()-1);
    LOG("MAC: \t\t" << macstr << " " << mac.size() << " " << mac);
    LOG(ZUID().str());

    //ZArray<ZUID> uidlist;
    //for(zu64 i = 0; i < 20; ++i){
    //    uidlist.push(ZUID());
    //}
    //for(zu64 i = 0; i < uidlist.size(); ++i){
    //    LOG(uidlist[i].str());
    //}

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
        if(hash1a != hash1b)
            throw __LINE__;

        zu64 hash2a = ZHash<ZString, ZHashBase::xxHash64>(data).hash();
        zu64 hash2b = ZHash<ZString, ZHashBase::xxHash64>(data).hash();
        LOG("XXHash: " << data << " " << hash2a << " " << hash2b);
        if(hash2a != hash2b)
            throw __LINE__;

        zu64 hash3a = ZHash<ZString, ZHashBase::fnvHash64>(data).hash();
        zu64 hash3b = ZHash<ZString, ZHashBase::fnvHash64>(data).hash();
        LOG("FNVHash: " << data << " " << hash3a << " " << hash3b);
        if(hash3a != hash3b)
            throw __LINE__;
    };


    ZString data2 = "hashdata";
    ZString data3 = "hashdata1";
    ZString data4 = "hashdata2";

    hf(data2);
    hf(data3);
    hf(data4);

    return 0;
}
