#include "test.h"

#include "zarray.h"

int array_block(){
    LOG("=== Array Test...");
    ZArray<int> tst;
    tst.push(10);
    tst.pushFront(45);
    tst.push(567);
    for(unsigned i = 0; i < tst.size(); ++i){
        LOG(tst[i]);
    }

    int myints[] = {16,2,77,29};
    LOG((zu64)myints << " " << sizeof(myints) << " " << sizeof(int));
    ZArray<int> tst2(myints, 4);
    for(unsigned i = 0; i < tst2.size(); ++i){
        LOG(tst2[i]);
    }

    char mychars[] = {'a', 'b', 'c', 'd', 'e'};
    LOG((zu64)mychars << " " << sizeof(mychars) << " " << sizeof(char));
    ZArray<char> tst3(mychars, 4);
    for(unsigned i = 0; i < tst3.size(); ++i){
        LOG(ZString(tst3[i]));
    }

    const char* c1 = "test chars";
    ZArray<char> t1(c1, 10);
    t1.push('!');
    t1.push('\0');
    OLOG("'" << t1.ptr() << "'");
    for(zu64 i = 0; i < t1.size(); ++i)
        OLOG(ZLog::noln << (char)t1[i] << ".");
    ORLOG(ZLog::newln);
    return 0;
}

int assoc_block(){
    LOG("=== Assoc Test...");
    ZAssoc<ZString, ZString> zarr;
    zarr["here"] = "there";
    zarr.pushFront("this", "that");
    zarr.push("where?");
    for(unsigned i = 0; i < zarr.size(); ++i){
        LOG(zarr.key(i) << " " << zarr[i]);
    }
    return 0;
}
