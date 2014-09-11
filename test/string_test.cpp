#include "test.h"

#include "zstring.h"
#include "zpath.h"

int string_block(){
    ZString str1;
    ZString str2("A String!");
    ZString str3("test3");
    ZString str4("test4");
    ZString str5("test3test4");

    if(!(str1 != str2))
        throw 1;

    str1 = str2;
    if(!(str1 == str2))
        throw 2;

    ZString str6 = str3.concat(str4);
    if(str6 != str5)
        throw 3;

    str3.append(str4);
    if(str3 != str5)
        throw 4;

//    ZString(std::string);
//    std::string &str();

//    ZString(std::wstring);
//    std::wstring wstr() const;

//    //ZString(char*);
//    //char *c();

//    ZString(const unsigned char *);
//    ZString(const char *);
//    const char *cc() const;

//    ZString(char, zu64 len = 1);

//    ZString(zu16);
//    ZString(zs16);
//    ZString(zu32);
//    ZString(zs32);
//    ZString(zint);
//    ZString(zuint);
//    //explicit ZString(zsint);
//    ZString(zu64);
//    ZString(zs64);

//    static ZString ItoS(zu64 num, unsigned base = 10, zu64 pad = 0);
//    static ZString ItoS(zs64 num, unsigned base = 10);
//    int tint() const;

//    ZString(double flt, unsigned places = -1);

//    ZString(ZArray<char> bin);

//    char &operator[](zu64);

//    zu64 size() const;
//    inline zu64 length() const { return size(); }
//    zu64 count(ZString) const;

//    char first() const;
//    char last() const;

//    void clear();
//    bool isEmpty() const;

//    // Tests if <str> begins with <test>. Ignores whitespace at beginning of string if <ignore_whitespace>
//    bool startsWith(ZString test, bool ignore_whitespace = true) const;
//    // Alias for startsWith, always ignores whitespace
//    inline bool beginsWith(ZString test) const { return startsWith(test, false); }

//    // Tests if <str> ends with <test>
//    bool endsWith(ZString test) const;

//    // Insert character at direction
//    ZString &insert(zu64 pos, ZString txt);
//    static ZString insert(ZString str, zu64 pos, ZString txt);

//    // Get portion of <str> from <pos> to end
//    ZString &substr(zu64 pos);
//    static ZString substr(ZString str, zu64 pos);

//    // Get <len> characters after <pos> of <str>
//    ZString &substr(zu64 pos, zu64 len);
//    static ZString substr(ZString str, zu64 pos, zu64 len);

//    // Get location of first character of first occurrence of <find> in <str>
//    static zu64 findFirst(ZString str, ZString find);

//    // Replace section <len> characters long at <pos> with <after> in <str>
//    ZString &replace(zu64 pos, zu64 len, ZString after);
//    static ZString replace(ZString str, zu64 pos, zu64 len, ZString after);

//    // Replace the first occurrence of <before> in <str> with <after>, up to <max> times
//    ZString &replaceRecursive(ZString before, ZString after, unsigned max = 1000);
//    static ZString replaceRecursive(ZString str, ZString before, ZString after, unsigned max = 1000);

//    // Replace up to <max> occurences of <before> with <after> in <str>
//    // <max> = -1 for unlimited
//    ZString &replace(ZString before, ZString after, unsigned max = -1);
//    static ZString replace(ZString str, ZString before, ZString after, unsigned max = -1);

//    // Get sub-string of <str> before first occurence of <find> in <str>
//    static ZString getUntil(ZString str, ZString find);

//    ZString findFirstBetween(ZString, ZString);
//    ZString replaceBetween(ZString start, ZString end, ZString after);
//    ZString findFirstXmlTagCont(ZString tag);
//    ZString replaceXmlTagCont(ZString tag, ZString after);
//    ZString label(ZString label, ZString value, bool modify = true);
//    ZString label(AsArZ, bool modify = true);

//    // Strip occurences of <target> from beginning and end of <str>
//    ZString &strip(char target);
//    static ZString strip(ZString str, char target);

//    ZString removeWhitespace();

//    ZString invert(bool modify = true);

//    // Convert UPPERCASE characters to lowercase equivalents in <str>
//    ZString &toLower();
//    static ZString toLower(ZString str);

//    ZString duplicate(unsigned iterate, bool modify = true);
//    ZString popLast();
//    static ZString compound(ArZ parts, ZString delim);

//    ArZ split(ZString delim);

//    ArZ explode(char delim);
//    ArZ explodeList(unsigned nargs, ...);
//    ArZ strict_explode(char delim);
//    ArZ explode();

//    bool isUtf8(ZString);

//    static bool alphaTest(ZString str1, ZString str2);

//    // Allows ZString to be used with std streams
//    friend std::ostream &operator<<(std::ostream& lhs, ZString rhs);

    return 0;
}

int string_block_old(){
    LOG("=== String Test...");
    ZString t3 = "hello, this is a string";
    LOG(t3.size() << " " << t3);
    t3.append(", and this is appended");
    LOG(t3.size() << " " << t3);
    t3.replace(" is ", " \"is still\" ");
    LOG(t3.size() << " " << t3);
    ZArray<ZString> words = t3.explode(' ');
    for(zu64 i = 0; i < words.size(); ++i)
        LOG('-' << words[i] << "- " << ZLog::noln);
    LOG(ZLog::newln);
    words.concat(t3.strict_explode(' '));
    for(zu64 i = 0; i < words.size(); ++i)
        LOG('-' << words[i] << "- " << ZLog::noln);
    LOG(ZLog::newln);

    LOG("this text here");
    ZString tst;
    tst << 'a';
    tst << '5';
    tst << ' ';
    tst << 'g';
    tst << "7";
    LOG(tst);
    LOG("more text here");
    return 0;
}

int string_magic_block(){
    LOG("=== String Magic Test...");
    ZString tst = "this and this text";
    tst.replace("this", "xthisx", 4);
    LOG(tst);
    //ZString tst2 = "that that that text";
    //tst2.replaceEach("that", "taat", 1);
    //LOG(tst2);
    ZString tst3 = "sdfgdfgdfgdfgdfgdfg";
    tst3.replace(3, 5, "ZZZZ");
    LOG(tst3);
    return 0;
}

int path_block(){
    LOG("=== Path Test...");
    // /a/path/here/test.txt
    // /this/path/to/another/place
    // ../../../../../a/path/here/test.txt

    // /some/short/thing.obj
    // /some/short/other/path
    // ../../thing.obj

    ZPath p1 = ZString("/some/odd/../complic/ated/../path");
    ZPath p2 = ZString("/"); // ../../thing.obj
    p1.relTo(p2);
    LOG(p1);
    LOG(p2);

    ZPath pc = ZString("/some/odd/../complic/ated/../path");
    pc.sanitize();
    LOG(pc);
    pc = "../../another/path";
    pc.sanitize();
    LOG(pc);
    pc = "/some/../../../path";
    pc.sanitize();
    LOG(pc);
    return 0;
}
int path_windows_block(){
    LOG("=== Path Windows Test...");
    ZPath here = ZPath::pwd();
    LOG(here.size() << " " << here.str());

//    ZPath mk = "over/there/lol.txt";
//    LOG(mk);
//    if(mk.createDirsTo())
//        LOG("ok");
//    else
//        LOG("no");
    return 0;
}
