#include "test.h"
#include "znumber.h"
#include "zbinary.h"
#include <bitset>

using namespace LibChaos;

int number_block(){

    zu32 num1 = 453;
    std::bitset<32> b1(num1);
    ZString str1 = b1.to_string();
    for(zu64 i = 0; i < str1.size(); i++){
        i += 8;
        str1.insert(i, " ");
    }
    LOG(str1 << num1);

    zu64 num2 = (zu64)num1;
    std::bitset<64> b2(num2);
    ZString str2 = b2.to_string();
    for(zu64 i = 0; i < str2.size(); i++){
        i += 8;
        str2.insert(i, " ");
    }
    LOG(str2 << num2);

    ZString out;
    zbyte *ptr = (zbyte*)&num1;
    for(zu32 i = 0; i < sizeof(num1); ++i){
        std::bitset<8> b1((zbyte)ptr[i]);
        out << b1.to_string() << " ";
    }
    LOG(out << num1);

    ZNumber number1;
    LOG("Number 1: " << number1.str() << " - " << number1.strBytes());
    number1 = num1;
    LOG("Number 1: " << number1.str() << " - " << number1.strBytes());
    //LOG("Factorial 1: " << number1.factorial().str());

    return 0;
}
