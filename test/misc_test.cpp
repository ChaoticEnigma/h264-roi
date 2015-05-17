#include "test.h"
#include "zuid.h"
#include "zlist.h"
#include "zhash.h"

int uid_test(){
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

    ZString data2 = "hashdata";
    ZString data3 = "hashdata1";
    ZString data4 = "hashdata2";

    zu64 hash2 = ZHash<ZString>(data2).hash();
    zu64 hash3 = ZHash<ZString>(data3).hash();
    zu64 hash4 = ZHash<ZString>(data4).hash();

    LOG(data2 << " " << hash2);
    LOG(data3 << " " << hash3);
    LOG(data4 << " " << hash4);

    zu64 hash5 = ZHash<ZString, ZHashBase::xxHash64>(data2).hash();
    zu64 hash6 = ZHash<ZString, ZHashBase::xxHash64>(data3).hash();
    zu64 hash7 = ZHash<ZString, ZHashBase::xxHash64>(data4).hash();

    LOG(data2 << " " << hash5);
    LOG(data3 << " " << hash6);
    LOG(data4 << " " << hash7);

    return 0;
}
