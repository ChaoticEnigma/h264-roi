#include "test.h"
#include "zmap.h"
#include "zset.h"

namespace LibChaosTest {

void hash(){
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
}

void map(){
    ZMap<ZString, zu64> map1;
    map1.add("test1", 11);
    map1.add("test2", 22);
    map1.add("test3", 33);
    map1.add("test4", 44);
    map1.add("test5", 55);
    map1.add("test6", 66);
    map1.add("test7", 77);
    map1.add("test8", 88);
    map1.add("test9", 99);
    map1.add("test:", 111);
    map1.add("test;", 222);
    map1.add("test<", 333);
    map1.add("test=", 444);
    map1.add("test>", 555);
    map1.add("test?", 666);

    LOG(map1.get("test4"));
    LOG(map1.get("test3"));
    LOG(map1.get("test2"));
    LOG(map1.get("test1"));

    map1.add("test3", 3333);

    LOG(map1.get("test3"));

    LOG(map1.size() << " " << map1.realSize());
    for(zu64 i = 0; i < map1.realSize(); ++i){
        if(map1.position(i).flags & ZMAP_ENTRY_VALID)
            LOG(i << " " << map1.position(i).key << " " << map1.position(i).value << " " << map1.position(i).hash);
    }

    map1["test21"] = 1001;
    map1["test22"] = 2002;

    LOG(map1.get("test21"));
    LOG(map1.get("test22"));

    map1.remove("test8");
    map1.remove("test:");

    LOG(map1.size() << " " << map1.realSize());
    for(zu64 i = 0; i < map1.realSize(); ++i){
        if(map1.position(i).flags & ZMAP_ENTRY_VALID)
            LOG(i << " " << map1.position(i).key << " " << map1.position(i).value << " " << map1.position(i).hash);
    }

    map1["test8"] = 888;

    LOG(map1.size() << " " << map1.realSize());
    for(zu64 i = 0; i < map1.realSize(); ++i){
        if(map1.position(i).flags & ZMAP_ENTRY_VALID)
            LOG(i << " " << map1.position(i).key << " " << map1.position(i).value << " " << map1.position(i).hash);
    }

    map1["test8"] = 999;

    LOG(map1.size() << " " << map1.realSize());
    for(zu64 i = 0; i < map1.realSize(); ++i){
        if(map1.position(i).flags & ZMAP_ENTRY_VALID)
            LOG(i << " " << map1.position(i).key << " " << map1.position(i).value << " " << map1.position(i).hash);
    }

    LOG("Forward Iterator: " << map1.size());
    auto i1f = map1.begin();
    test_forward_iterator(&i1f, map1.size());

    ZMap<ZString, zu64> map2 = map1;
    LOG("Forward Iterator: " << map2.size());
    auto i2f = map2.begin();
    test_forward_iterator(&i2f, map2.size());
}

void set(){
    ZString str1 = "one";
    ZString str2 = "two";
    ZString str3 = "three";
    ZString str4 = "four";
    ZString str5 = "five";

    ZSet<ZString> set1;
    set1.add(str1);
    set1.add(str2);
    set1.add(str3);
    set1.add(str4);
    set1.add(str5);
    set1.remove(str3);
    TASSERT(set1.contains(str1));
    TASSERT(set1.contains(str2));
    TASSERT(!set1.contains(str3));
    TASSERT(set1.contains(str4));
    TASSERT(set1.contains(str5));

    LOG("Forward Iterator: " << set1.size());
    auto i1f = set1.begin();
    test_forward_iterator(&i1f, set1.size());

    ZSet<ZString> set2 = set1;
    LOG("Forward Iterator: " << set2.size());
    auto i2f = set1.begin();
    test_forward_iterator(&i2f, set2.size());
}

ZArray<Test> hash_tests(){
    return {
        { "hash",   hash,   true, {} },
        { "map",    map,    true, {} },
        { "set",    set,    true, {} },
    };
}

}
