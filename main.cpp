#include <iostream>
#include <zarray.h>
#include <zstring.h>
using namespace std;

int main(){
    ZArray<char> tst;
    tst.push_back('a');
    tst.push_back('c');
    cout << tst[1] << tst[0] << " " << tst.size() << endl;
    cout << tst.c_style() << endl;

    ZArray<char> tst2("thishere", 8);
    cout << tst2[0] << " " << tst2.size() << " " << tst2.c_style() << endl;

    ZString test("hereisme");
    cout << test << endl;
    return 0;
}
