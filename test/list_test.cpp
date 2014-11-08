#include "test.h"
#include "zlist.h"
#include "zerror.h"

int list_block(){
    ZList<int> tst;
    // push some values
    tst.pushBack(10);
    LOG(tst.debug());
    tst.pushBack(20);
    LOG(tst.debug());
    tst.pushFront(45);
    LOG(tst.debug());
    tst.pushBack(567);
    LOG(tst.debug());

    // push to back and pop back
    tst.pushBack(663);
    LOG(tst.debug());
    tst.popBack();
    LOG(tst.debug());

    // push to front and pop front
    tst.pushFront(336);
    LOG(tst.debug());
    tst.popFront();
    LOG(tst.debug());

    if(tst.size() == 4 && tst[0] == 45 && tst[1] == 10 && tst[2] == 20 &&  tst[3] == 567)
        LOG(tst.size() << " " << tst[0] << "." << tst[1] << "." << tst[2] << "." << tst[3] << " OK");
    else
        throw (int)1;

    int myints[] = { 16, 2, 77, 29 };
    ZList<int> tst2(myints, 4);
    if(tst2.size() == 4 && tst2[0] == 16 && tst2[1] == 2 && tst2[2] == 77 && tst2[3] == 29)
        LOG(tst2.size() << " " << tst2[0] << "." << tst2[1] << "." << tst2[2] << "." << tst2[3] << " OK");
    else
        throw (int)2;

    char mychars[] = { 'a', 'b', 'c', 'd', 'e' };
    ZList<char> tst3(mychars, 5);
    if(tst3.size() == 5 && tst3[0] == 'a' && tst3[1] == 'b' && tst3[2] == 'c' && tst3[3] == 'd' && tst3[4] == 'e')
        LOG(tst3.size() << " " << tst3[0] << "." << tst3[1] << "." << tst3[2] << "." << tst3[3] << "." << tst3[4] << " OK");
    else
        throw (int)3;

    ZList<ZString> tst4;
    tst4.push("three");
    tst4.pushFront("two");
    tst4.push("four");
    tst4.pushFront("one");
    tst4.push("five");

    if(tst4.size() == 5 && tst4[0] == "one" && tst4[1] == "two" && tst4[2] == "three" && tst4[3] == "four" && tst4[4] == "five")
        LOG(tst4.size() << " " << tst4[0] << "." << tst4[1] << "." << tst4[2] << "." << tst4[3] << "." << tst4[4] << " OK");
    else
        throw (int)4;

    return 0;
}
