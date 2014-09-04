#include "test.h"
#include "znumber.h"
#include "zbinary.h"
#include <bitset>

using namespace LibChaos;

int number_block(){

    zs64 num1 = 455;
    std::bitset<64> b1(num1);
    ZString str1 = b1.to_string();
    for(zu64 i = 0; i < str1.size(); i++){
        i += 8;
        str1.insert(i, " ");
    }
    LOG(str1 << num1);

    zs64 num2 = -455;
    std::bitset<64> b2(num2);
    ZString str2 = b2.to_string();
    for(zu64 i = 0; i < str2.size(); i++){
        i += 8;
        str2.insert(i, " ");
    }
    LOG(str2 << num2);

    ZNumber number1;
    LOG("Number 1: " << number1.str());

    return 0;
}
