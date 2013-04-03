#include <iostream>
#include <zarray.h>
#include <zstring.h>
using namespace std;

int main(){
    ZArray<char> tst2("thishere", 8);
    tst2.push_back(NULL);
    ZArray<char> tst3("thishereis", 10);
    tst3.push_back(NULL);
    tst2 = tst3;
    tst3.drop_back();
    tst3.push_back('x');
    tst3.push_back(NULL);
    if(tst2 != tst3)
    cout << tst2.size() << " " << tst2.c_style() << endl;
    cout << tst3.size() << " " << tst3.c_style() << endl;

    ZString test("hereisme");
    cout << test << endl;

    const char * c4 = "thatstring";
    ZArray<char> tst4(c4, strlen(c4)+1);
    cout << tst4.size() << " " << tst4.c_style() << endl;

    ZString t3 = "hello, this is a string";
    cout << t3.size() << " " << t3 << endl;
    t3.append(", and this is appended");
    cout << t3.size() << " " << t3 << endl;
    cout << t3.count("is") << endl;

    ZArray<ZString::SubZStr> pos = t3.findAll(" is ");
    for(unsigned i = 0; i < pos.size(); ++i)
        cout << pos[i].start << "," << pos[i].end << " ";
    cout << endl;

    //t3.replace(" is ", " is still ");
    cout << t3.size() << " " << t3 << endl;

    return 0;
}
