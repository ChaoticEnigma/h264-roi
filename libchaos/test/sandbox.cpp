#include "test.h"

#include <iostream>

struct MyClass {
    static int count;

    ZString num;
    MyClass(){
        num = ++count;
        LOG("MyClass " << num << " constructed");
    }
//    ~MyClass(){
//        LOG("MyClass " << num << " destroyed");
//    }
    MyClass(const MyClass &other){
        num = ++count;
        num << "-" << other.num;
        LOG("MyClass " << num << " copied");
    }
    MyClass(MyClass &&other){
        num = ++count;
        num << "=" << other.num;
        other.num = 0;
        LOG("MyClass " << num << " moved");
    }
};

int MyClass::count = 0;

int sandbox(){

    MyClass obj;

//    MyClass *pt = new MyClass[3];
//    delete[] pt;

    MyClass obj2 = MyClass();

    MyClass obj3 = obj2;

    //pt = new MyClass[3];
    //operator delete pt;

    return 0;
}
