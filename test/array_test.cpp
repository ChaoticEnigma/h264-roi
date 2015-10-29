#include "test.h"
#include "zarray.h"
#include "zstack.h"

int array_test(){
    ZArray<int> tst;
    tst.push(10);
    tst.pushFront(45);
    tst.push(567);
    TASSERT(tst.size() == 3 && tst[0] == 45 && tst[1] == 10 && tst[2] == 567);
    LOG(tst.size() << " " << tst[0] << "." << tst[1] << "." << tst[2] << " OK");

    int myints[] = { 16, 2, 77, 29 };
    ZArray<int> tst2(myints, 4);
    TASSERT(tst2.size() == 4 && tst2[0] == 16 && tst2[1] == 2 && tst2[2] == 77 && tst2[3] == 29);
    LOG(tst2.size() << " " << tst2[0] << "." << tst2[1] << "." << tst2[2] << "." << tst2[3] << " OK");

    ZArray<char> tst3({ 'a', 'b', 'c', 'd', 'e' });
    TASSERT(tst3.size() == 5 && tst3[0] == 'a' && tst3[1] == 'b' && tst3[2] == 'c' && tst3[3] == 'd' && tst3[4] == 'e');
    LOG(tst3.size() << " " << tst3[0] << "." << tst3[1] << "." << tst3[2] << "." << tst3[3] << "." << tst3[4] << " OK");

    ZArray<ZString> tst4({ "one", "two", "four", "five" });
    tst4.insert(2, "three");
    TASSERT(tst4.size() == 5 && tst4[0] == "one" && tst4[1] == "two" && tst4[2] == "three" && tst4[3] == "four" && tst4[4] == "five");
    LOG(tst4.size() << " " << tst4[0] << "." << tst4[1] << "." << tst4[2] << "." << tst4[3] << "." << tst4[4] << " OK");

    ZArray<ZString> tst5({ "one", "junk1", "two", "three", "junk2", "four", "five" });
    tst5.erase(1);
    tst5.erase(3);
    TASSERT(tst5.size() == 5 && tst5[0] == "one" && tst5[1] == "two" && tst5[2] == "three" && tst5[3] == "four" && tst5[4] == "five");
    LOG(tst5.size() << " " << tst5[0] << "." << tst5[1] << "." << tst5[2] << "." << tst5[3] << "." << tst5[4] << " OK");

    tst5.reverse();
    TASSERT(tst5.size() == 5 && tst5[0] == "five" && tst5[1] == "four" && tst5[2] == "three" && tst5[3] == "two" && tst5[4] == "one");
    LOG(tst5.size() << " " << tst5[0] << "." << tst5[1] << "." << tst5[2] << "." << tst5[3] << "." << tst5[4] << " OK");

    // Iterators

    LOG("Forward Iterator: " << tst4.size());
    auto i4f = tst4.begin();
    test_forward_iterator(&i4f, tst4.size());

    LOG("Reverse Iterator: " << tst4.size());
    auto i4r = tst4.end();
    test_reverse_iterator(&i4r, tst4.size());

    LOG("Duplex Iterator:");
    auto i4d = tst4.begin();
    test_duplex_iterator(&i4d, tst4.size());

    LOG("Random Iterator:");
    auto i4n = tst4.begin();
    test_random_iterator(&i4n, tst4.size());

    return 0;
}

int assoc_test(){
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

int stack_test(){
    ZStack<int> tst1;

    zu64 u = 56546;
    int i = u;
    LOG(i);

    return 0;
}
