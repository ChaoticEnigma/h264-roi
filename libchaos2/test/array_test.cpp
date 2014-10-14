#include "test.h"
#include "zarray.h"
#include "zerror.h"

int array_block(){
    ZArray<int> tst;
    tst.push(10);
    tst.pushFront(45);
    tst.push(567);
    if(tst.size() == 3 && tst[0] == 45 && tst[1] == 10 && tst[2] == 567)
        LOG(tst.size() << " " << tst[0] << "." << tst[1] << "." << tst[2] << " OK");
    else
        throw (int)1;

    int myints[] = { 16, 2, 77, 29 };
    ZArray<int> tst2(myints, 4);
    if(tst2.size() == 4 && tst2[0] == 16 && tst2[1] == 2 && tst2[2] == 77 && tst2[3] == 29)
        LOG(tst2.size() << " " << tst2[0] << "." << tst2[1] << "." << tst2[2] << "." << tst2[3] << " OK");
    else
        throw (int)2;

    char mychars[] = { 'a', 'b', 'c', 'd', 'e' };
    ZArray<char> tst3(mychars, 5);
    if(tst3.size() == 5 && tst3[0] == 'a' && tst3[1] == 'b' && tst3[2] == 'c' && tst3[3] == 'd' && tst3[4] == 'e')
        LOG(tst3.size() << " " << tst3[0] << "." << tst3[1] << "." << tst3[2] << "." << tst3[3] << "." << tst3[4] << " OK");
    else
        throw (int)3;

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
