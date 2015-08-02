#include "test.h"
#include "znumber.h"
#include "zbinary.h"
#include <bitset>

#define printnum(A) LOG(ZString::ItoS((zu64)(A), 2, 64) << " " << (A))

//void printnum(zu64 num){
//    std::bitset<32> b1(num);
//    ZString str1 = b1.to_string();
//    for(zu64 i = 0; i < str1.size(); i++){
//        i += 8;
//        str1.insert(i, " ");
//    }
//    LOG(str1 << num);
//}

int number_test(){

    zu32 num1 = 453;
    printnum(num1);
    printnum(num1 + num1);

//    ZString out;
//    zbyte *ptr = (zbyte*)&num1;
//    for(zu32 i = 0; i < sizeof(num1); ++i){
//        std::bitset<8> b1((zbyte)ptr[i]);
//        out << b1.to_string() << " ";
//    }
//    LOG(out << num1);

    ZNumber number1;
    LOG("Number 1: " << number1.str() << " - " << number1.size() << " - " << number1.strBytes());
    number1 = num1;
    LOG("Number 1: " << number1.str() << " - " << number1.size() << " - " << number1.strBytes());
    number1 += num1;
    LOG("Number 1: " << number1.str() << " - " << number1.size() << " - " << number1.strBytes());
    number1 += 1;
    LOG("Number 1: " << number1.str() << " - " << number1.size() << " - " << number1.strBytes());

    zu64 num2 = 8;
    num2 >>= 4;
    ZNumber number2 = num2;
    LOG("Number 2: " << number2.str() << " - " << number2.size() << " - " << number2.strBytes());

    ZNumber number3 = -1;
    LOG("Number 3: " << number3.str() << " - " << number3.size() << " - " << number3.strBytes());
    //LOG("Factorial 1: " << number1.factorial().str());

    // Repeating floating points to test:
    // 1 / 3 = 0.[3] = 0.333...
    // 2 / 3 = 0.[6] = 0.666...
    // 7 / 12 = 0.58[3] = 0.58333...
    // 1 / 81 = 0.[012345679] = 0.012345679012345679012345679...
    // 22 / 7 = 3.[142857] = 3.142857142857142857...
    // 3227 / 555 = 5.8[144] = 5.8144144144...

    return 0;
}
