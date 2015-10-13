#include "test.h"
#include "zlist.h"
#include "zqueue.h"

int list_test(){
    ZList<int> list1;
    // push some values
    list1.pushBack(10);
    LOG(list1.debug());
    list1.pushBack(20);
    LOG(list1.debug());
    list1.pushFront(45);
    LOG(list1.debug());
    list1.pushBack(567);
    LOG(list1.debug());

    // push to back and pop back
    list1.pushBack(663);
    LOG(list1.debug());
    list1.popBack();
    LOG(list1.debug());

    // push to front and pop front
    list1.pushFront(336);
    LOG(list1.debug());
    list1.popFront();
    LOG(list1.debug());

    TASSERT(list1.size() == 4 && list1[0] == 45 && list1[1] == 10 && list1[2] == 20 &&  list1[3] == 567);
    LOG(list1.size() << " " << list1[0] << "." << list1[1] << "." << list1[2] << "." << list1[3] << " OK");

    int myints[] = { 16, 2, 77, 29 };
    ZList<int> list2(myints, 4);
    TASSERT(list2.size() == 4 && list2[0] == 16 && list2[1] == 2 && list2[2] == 77 && list2[3] == 29);
    LOG(list2.size() << " " << list2[0] << "." << list2[1] << "." << list2[2] << "." << list2[3] << " OK");

    char mychars[] = { 'a', 'b', 'c', 'd', 'e' };
    ZList<char> list3(mychars, 5);
    TASSERT(list3.size() == 5 && list3[0] == 'a' && list3[1] == 'b' && list3[2] == 'c' && list3[3] == 'd' && list3[4] == 'e');
    LOG(list3.size() << " " << list3[0] << "." << list3[1] << "." << list3[2] << "." << list3[3] << "." << list3[4] << " OK");

    ZList<ZString> list4;
    list4.push("three");
    list4.pushFront("two");
    list4.push("four");
    list4.pushFront("one");
    list4.push("five");

    TASSERT(list4.size() == 5 && list4[0] == "one" && list4[1] == "two" && list4[2] == "three" && list4[3] == "four" && list4[4] == "five");
    LOG(list4.size() << " " << list4[0] << "." << list4[1] << "." << list4[2] << "." << list4[3] << "." << list4[4] << " OK");

    LOG("Forward Iterator: " << list4.size());
    for(auto i = list4.begin(); i != list4.end(); ++i){
        LOG("i: " << *i);
    }
    LOG("Reverse Iterator: " << list4.size());
    for(auto i = list4.end(); i != list4.begin(); --i){
        LOG("i: " << *i);
    }

    return 0;
}

int queue_test(){
    ZQueue<int> tst1;

    return 0;
}
