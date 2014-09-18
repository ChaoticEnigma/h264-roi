#include "test.h"

#include "zstring.h"
#include "zpath.h"

int string_block(){
    ZString str1;
    ZString str2("A String!");
    ZString str3("test3");
    ZString str4("test4");
    ZString str5("test3test4");

    LOG("-- Assign / Compare:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    if(!(str1 != str2))
        throw 1;

    str1 = str2;
    LOG(str1);
    if(!(str1 == str2))
        throw 2;

    LOG("-- Concat / Append:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString str6 = str3.concat(str4);
    LOG(str6);
    if(str6 != str5)
        throw 3;

    str3.append(str4);
    LOG(str3);
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

    LOG("-- Find:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString find1("someInterestingString");
    zu64 pos1 = find1.findFirst("est");
    LOG(pos1);
    if(pos1 != 9)
        throw 31;

    ZString find2("someInterestingString");
    zu64 pos2 = ZString::findFirst(find1, "est");
    LOG(pos2);
    if(pos2 != 9)
        throw 32;

    ZString find3("anotherInterestingStringWithInterestingThings");
    ZArray<zu64> pos3 = find3.findAll("est");
    LOG("" << ZLog::noln);
    FOREACH(pos3.size()){
        RLOG(pos3[i] << " ");
    }
    RLOG(ZLog::newln);
    if(pos3.size() != 2 || pos3[0] != 12 || pos3[1] != 33)
        throw 33;

    ZString find4("anotherInterestesteingStringWithInterestesteingThings");
    ZArray<zu64> pos4 = ZString::findAll(find4, "este");
    LOG("" << ZLog::noln);
    FOREACH(pos4.size()){
        RLOG(pos4[i] << " ");
    }
    RLOG(ZLog::newln);
    if(pos4.size() != 2 || pos4[0] != 12 || pos4[1] != 37)
        throw 34;

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

    LOG("-- Explode / Compound:"); // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString strarr = "this!will!explode!";
    ArZ arr1 = strarr.explode('!');
    ZString cmp1 = ZString::compound(arr1, "-");
    LOG(cmp1);
    if(!(arr1.size() == 3 && arr1[0] == "this" && arr1[1] == "will" && arr1[2] == "explode"))
        throw 45;
    if(cmp1 != "this-will-explode")
        throw 46;

    ZString strarr2 = "this!will!\"sort!of\"!explode\"strstr\"!";
    ArZ arr2 = strarr2.quotedExplode('!');
    ZString cmp2 = ZString::compound(arr2, "-");
    LOG(cmp2);
    if(!(arr2.size() == 5 && arr2[0] == "this" && arr2[1] == "will" && arr2[2] == "sort!of" && arr2[3] == "explode" && arr2[4] == "strstr"))
        throw 47;
    if(cmp2 != "this-will-sort!of-explode-strstr")
        throw 48;

    ZString strarr3 = "\\!\\!!!this!will\\!also!explode\"strstr\"!";
    ArZ arr3 = strarr3.escapedExplode('!');
    ZString cmp3 = ZString::compound(arr3, "-");
    LOG(cmp3);
    if(!(arr3.size() == 4 && arr3[0] == "\\!\\!" && arr3[1] == "this" && arr3[2] == "will\\!also" && arr3[3] == "explode\"strstr\""))
        throw 49;
    if(cmp3 != "\\!\\!-this-will\\!also-explode\"strstr\"")
        throw 50;

    ZString strarr4 = "this!.!will!.!explode!.!differently\"strstr\"";
    ArZ arr4 = strarr4.strExplode("!.!");
    ZString cmp4 = ZString::compound(arr4, "---");
    LOG(cmp4);
    if(!(arr4.size() == 4 && arr4[0] == "this" && arr4[1] == "will" && arr4[2] == "explode" && arr4[3] == "differently\"strstr\""))
        throw 51;
    if(cmp4 != "this---will---explode---differently\"strstr\"")
        throw 52;

//    ArZ explodeList(unsigned nargs, ...);

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
    //words.concat(t3.strict_explode(' '));
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
