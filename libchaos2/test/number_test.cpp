#include "test.h"
#include "znumber.h"
#include "zbinary.h"
#include <bitset>

void printnum(zu64 num){
    std::bitset<32> b1(num);
    ZString str1 = b1.to_string();
    for(zu64 i = 0; i < str1.size(); i++){
        i += 8;
        str1.insert(i, " ");
    }
    LOG(str1 << num);
}

int number_block(){

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

    return 0;
}
