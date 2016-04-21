#include "test.h"
#include "zlist.h"
#include "zqueue.h"

namespace LibChaosTest {

void list_push_pop(){
    ZList<int> list1;
    // push some values
    list1.pushBack(10);
    //LOG(list1.debug());
    list1.pushBack(20);
    //LOG(list1.debug());
    list1.pushFront(45);
    //LOG(list1.debug());
    list1.pushBack(567);
    //LOG(list1.debug());

    // push to back and pop back
    list1.pushBack(663);
    //LOG(list1.debug());
    list1.popBack();
    //LOG(list1.debug());

    // push to front and pop front
    list1.pushFront(336);
    //LOG(list1.debug());
    list1.popFront();
    //LOG(list1.debug());

    TASSERT(list1.size() == 4 && list1[0] == 45 && list1[1] == 10 && list1[2] == 20 &&  list1[3] == 567);
    LOG(list1.size() << " " << list1[0] << "." << list1[1] << "." << list1[2] << "." << list1[3] << " OK");
}

void list_construct(){
    int myints[] = { 16, 2, 77, 29 };
    ZList<int> list2(myints, 4);
    TASSERT(list2.size() == 4 && list2[0] == 16 && list2[1] == 2 && list2[2] == 77 && list2[3] == 29);
    LOG(list2.size() << " " << list2[0] << "." << list2[1] << "." << list2[2] << "." << list2[3] << " OK");

    char mychars[] = { 'a', 'b', 'c', 'd', 'e' };
    ZList<char> list3(mychars, 5);
    TASSERT(list3.size() == 5 && list3[0] == 'a' && list3[1] == 'b' && list3[2] == 'c' && list3[3] == 'd' && list3[4] == 'e');
    LOG(list3.size() << " " << list3[0] << "." << list3[1] << "." << list3[2] << "." << list3[3] << "." << list3[4] << " OK");
}

void list_push_obj(){
    ZList<ZString> list4;
    list4.push("three");
    list4.pushFront("two");
    list4.push("four");
    list4.pushFront("one");
    list4.push("five");

    TASSERT(list4.size() == 5 && list4[0] == "one" && list4[1] == "two" && list4[2] == "three" && list4[3] == "four" && list4[4] == "five");
    LOG(list4.size() << " " << list4[0] << "." << list4[1] << "." << list4[2] << "." << list4[3] << "." << list4[4] << " OK");
}

void list_iterator(){
    ZList<ZString> list4;
    list4.push("three");
    list4.pushFront("two");
    list4.push("four");
    list4.pushFront("one");
    list4.push("five");

    LOG("Forward Iterator: " << list4.size());
    auto i4f = list4.begin();
    test_forward_iterator(&i4f, list4.size());

    LOG("Reverse Iterator: " << list4.size());
    auto i4r = list4.end();
    test_reverse_iterator(&i4r, list4.size());

    LOG("Duplex Iterator:");
    auto i4d = list4.begin();
    test_duplex_iterator(&i4d, list4.size());
}

void queue(){
    ZQueue<int> tst1;
}

ZArray<Test> list_tests(){
    return {
        { "list-push-pop",  list_push_pop,  true, {} },
        { "list-construct", list_construct, true, {} },
        { "list-push-obj",  list_push_obj,  true, {} },
        { "list-iterator",  list_iterator,  true, {} },
        { "queue",          queue,          true, {} },
    };
}

}
