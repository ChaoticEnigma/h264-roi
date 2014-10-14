#include "test.h"

int storage_block(){
    ZStorage *stor = new ZDefaultStorage;

    auto print = [&](){
        ZString str = "[";
        for(zu64 i = 0; i < stor->size(); ++i)
            str += ZString((zu64)stor->get(i)) + ",";
        str.substr(0, str.size()-1);
        str += "]";
        LOG(str);
    };

    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());

    stor->resize(10);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    for(zu64 i = 0; i < stor->size(); ++i)
        stor->set(i, (zbyte)i);
    print();

    stor->resize(11);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    for(zu64 i = 10; i < stor->size(); ++i)
        stor->set(i, 252);
    print();

    stor->resize(12);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    for(zu64 i = 11; i < stor->size(); ++i)
        stor->set(i, 253);
    print();

    stor->resize(21);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    for(zu64 i = 12; i < stor->size(); ++i)
        stor->set(i, 254);
    print();
    for(zu64 i = 0; i < stor->size(); ++i)
        stor->set(i, (zbyte)i);
    print();

    stor->resize(90);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    for(zu64 i = 21; i < stor->size(); ++i)
        stor->set(i, 255);
    print();

    stor->resize(40);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    print();

    stor->resize(350);
    LOG(stor->size() << " " << ((ZDefaultStorage*)stor)->realSize());
    for(zu64 i = 0; i < stor->size(); ++i)
        stor->set(i, (zbyte)i);
    print();

    delete stor;

    return 0;
}
