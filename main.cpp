#include <iostream>
#include <zarray.h>
#include <zstring.h>
using namespace std;

int main(){
    ZArray<char> tst;
    tst.push_back('a');
    cout << tst[0];
    tst.push_back('c');
    cout << tst[1] << endl;
    cout << tst.c_style() << endl;
    ZString test = "hereisme";
    cout << test << endl;
    return 0;
}
