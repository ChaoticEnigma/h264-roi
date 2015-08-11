#include "test.h"

#include "zstring.h"
#include "zpath.h"
#include <cmath>
#include <iostream>

int string_test(){
    ZString str1;
    ZString str2("A String!");
    ZString str3("test3");
    ZString str4("test4");
    ZString str5("test3test4");

    LOG("-- Assign / Compare:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    if(!(str1 != str2))
        throw __LINE__;

    str1 = str2;
    LOG(str1);
    if(!(str1 == str2))
        throw __LINE__;

    LOG("-- Concat / Append:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString str6 = str3.concat(str4);
    LOG(str6);
    if(str6 != str5)
        throw __LINE__;

    str3.append(str4);
    LOG(str3);
    if(str3 != str5)
        throw __LINE__;

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

    LOG("-- Integer to String:"); // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString itos1 = ZString::ItoS((zu64)4565464, 10);
    LOG(itos1);
    if(itos1 != "4565464")
        throw __LINE__;
    ZString itos2 = ZString::ItoS((zs64)-980948, 16);
    LOG(itos2);
    if(itos2 != "-ef7d4")
        throw __LINE__;

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

    LOG("-- Insert:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString insert1 = "youShouldThisSentence";
    ZString ins1 = ZString::insert(insert1, 9, "Complete");
    ZString ins1_1 = insert1.insert(9, "Complete");
    LOG(ins1);
    if(ins1 != "youShouldCompleteThisSentence" || ins1 != ins1_1)
        throw __LINE__;

    LOG("-- Substr:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString substr1 = "youShouldTakeTheRestOfThisString";
    ZString sub1 = ZString::substr(substr1, 16);
    ZString sub1_1 = substr1.substr(16);
    LOG(sub1);
    if(sub1 != "RestOfThisString" || sub1 != sub1_1)
        throw __LINE__;

    ZString substr2 = "youShouldTakeME!InThisString";
    ZString sub2 = ZString::substr(substr2, 13, 3);
    ZString sub2_1 = substr2.substr(13, 3);
    LOG(sub2);
    if(sub2 != "ME!" || sub2 != sub2_1)
        throw __LINE__;

    LOG("-- Find:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString find1("someInterestingString");
    zu64 pos1 = find1.findFirst("est");
    zu64 pos1_1 = ZString::findFirst(find1, "est");
    LOG(pos1);
    if(pos1 != 9 || pos1 != pos1_1)
        throw __LINE__;

    ZString find2 = "someInterestingStringIsInteresting";
    zu64 pos2 = ZString::findFirst(find2, "Int", 20);
    LOG(pos2);
    if(pos2 != 23)
        throw __LINE__;

    ZString find3 = "anotherInterestingStringWithInterestingThings";
    ZArray<zu64> pos3 = find3.findAll("est");
    LOG("" << ZLog::noln);
    FOREACH(pos3.size()){
        RLOG(pos3[i] << " ");
    }
    RLOG(ZLog::newln);
    if(pos3.size() != 2 || pos3[0] != 12 || pos3[1] != 33)
        throw __LINE__;

    ZString find4 = "anotherInterestesteingStringWithInterestesteingThings";
    ZArray<zu64> pos4 = ZString::findAll(find4, "este");
    LOG("" << ZLog::noln);
    FOREACH(pos4.size()){
        RLOG(pos4[i] << " ");
    }
    RLOG(ZLog::newln);
    if(pos4.size() != 2 || pos4[0] != 12 || pos4[1] != 37)
        throw __LINE__;

    LOG("-- Replace:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString replace1 = "anotherInterestingStringWithInterestingThings";
    ZString rep1 = ZString::replacePos(replace1, 5, 20, "!!!!!");
    LOG(rep1);
    if(rep1 != "anoth!!!!!ithInterestingThings")
        throw __LINE__;

    replace1.replacePos(0, 15, "!!!");
    LOG(replace1);
    if(replace1 != "!!!ingStringWithInterestingThings")
        throw __LINE__;

    ZString replace2 = "strposposposposdddddddd";
    ZString rep2 = ZString::replaceRecursive(replace2, "strpos", "bbbstr");
    LOG(rep2);
    if(rep2 != "bbbbbbbbbbbbstrdddddddd")
        throw __LINE__;

    ZString replace3 = "a";
    ZString rep3 = ZString::replaceRecursive(replace3, "a", "bad", 5);
    LOG(rep3);
    if(rep3 != "bbbbbaddddd")
        throw __LINE__;

    replace2.replaceRecursive("strpos", "posstr", 3);
    LOG(replace2);
    if(replace2 != "posposposstrposdddddddd")
        throw __LINE__;

    ZString repalce4 = "strstrsstrstrsstrs";
    ZString rep4 = ZString::replace(repalce4, "strs", "aaaa");
    LOG(rep4);
    if(rep4 != "aaaatrsaaaatrsaaaa")
        throw __LINE__;

    ZString replace5 = "ssssssssssssss";
    ZString rep5 = replace5.replace("ss", "t", 4);
    LOG(rep5);
    if(rep5 != "ttttssssss")
        throw __LINE__;

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

    ZString strarr = "this!will!!explode!";
    ArZ arr1 = strarr.explode('!');
    ZString cmp1 = ZString::compound(arr1, "-");
    LOG(cmp1);
    if(!(arr1.size() == 3 && arr1[0] == "this" && arr1[1] == "will" && arr1[2] == "explode"))
        throw __LINE__;
    if(cmp1 != "this-will-explode")
        throw __LINE__;

    ZString strarr2 = "this!will!\"sort!of\"!explode\"strstr\"!";
    ArZ arr2 = strarr2.quotedExplode('!');
    ZString cmp2 = ZString::compound(arr2, "-");
    LOG(cmp2);
    if(!(arr2.size() == 5 && arr2[0] == "this" && arr2[1] == "will" && arr2[2] == "sort!of" && arr2[3] == "explode" && arr2[4] == "strstr"))
        throw __LINE__;
    if(cmp2 != "this-will-sort!of-explode-strstr")
        throw __LINE__;

    ZString strarr3 = "\\!\\!!!this!will\\!also!!explode\"strstr\"!";
    ArZ arr3 = strarr3.escapedExplode('!');
    ZString cmp3 = ZString::compound(arr3, "-");
    LOG(cmp3);
    if(!(arr3.size() == 4 && arr3[0] == "\\!\\!" && arr3[1] == "this" && arr3[2] == "will\\!also" && arr3[3] == "explode\"strstr\""))
        throw __LINE__;
    if(cmp3 != "\\!\\!-this-will\\!also-explode\"strstr\"")
        throw __LINE__;

    ZString strarr4 = "this!.!will!.!explode!.!!.!differently\"strstr\"!.!";
    ArZ arr4 = strarr4.strExplode("!.!");
    ZString cmp4 = ZString::compound(arr4, "---");
    LOG(cmp4);
    if(!(arr4.size() == 4 && arr4[0] == "this" && arr4[1] == "will" && arr4[2] == "explode" && arr4[3] == "differently\"strstr\""))
        throw __LINE__;
    if(cmp4 != "this---will---explode---differently\"strstr\"")
        throw __LINE__;

    ZString strarr5 = "these!will.all!explode!";
    ArZ arr5 = strarr5.explodeList(2, '!', '.');
    ZString cmp5 = ZString::compound(arr5, "-");
    LOG(cmp5);
    if(!(arr5.size() == 4 && arr5[0] == "these" && arr5[1] == "will" && arr5[2] == "all" && arr5[3] == "explode"))
        throw __LINE__;
    if(cmp5 != "these-will-all-explode")
        throw __LINE__;

//    ArZ explodeList(unsigned nargs, ...);

//    bool isUtf8(ZString);

//    static bool alphaTest(ZString str1, ZString str2);

    ZString iterstr1 = "abcdefghijklmnopqrstuvwxyz";
    ZString iterstr2;
    //for(ZStringIterator i = iterstr1.begin(); !i.atEnd(); ++i){
    //    iterstr2 += *i;
    //}
    LOG(iterstr1);
    LOG(iterstr2);
    ZString iterstr3;
    //for(ZStringIterator i = iterstr1.end(); !i.atFront(); --i){
    //    iterstr3 += *i;
    //}
    LOG(iterstr3);

    LOG("-- Number Conversion:"); // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString numstr1 = "3345";
    zu64 num1 = numstr1.tozu64();
    LOG(numstr1 << " " << num1);
    if(num1 != 3345)
        throw __LINE__;

    ZString numstr2 = "546567867864512";
    zu64 num2 = numstr2.tozu64();
    LOG(numstr2 << " " << num2);
    if(num2 != 546567867864512)
        throw __LINE__;

    ZString numstr3 = "FF";
    zu64 num3 = numstr3.tozu64(16);
    LOG(numstr3 << " " << num3);
    if(num3 != 0xFF)
        throw __LINE__;

    ZString numstr4 = "ad562f";
    zu64 num4 = numstr4.tozu64(16);
    LOG(numstr4 << " " << num4);
    if(num4 != 0xAD562F)
        throw __LINE__;

    ZString floatstr3 = "45.223";
    float float3 = floatstr3.toFloat();
    LOG(floatstr3 << " " << float3);

    ZString floatstr4 = "-455";
    float float4 = floatstr4.toFloat();
    LOG(floatstr4 << " " << float4);


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
    tst3.replacePos(3, 5, "ZZZZ");
    LOG(tst3);
    return 0;
}
