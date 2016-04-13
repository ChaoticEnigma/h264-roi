#include "test.h"

#include "zstring.h"
#include "zpath.h"
#include <cmath>
#include <iostream>

int string_test();

ZList<Test> string_tests(){
    return {
        { "string-test", string_test, true }
    };
}

int string_test(){
    ZString str1;
    ZString str2("A String!");
    ZString str3("test3");
    ZString str4("test4");
    ZString str5("test3test4");

//    ZString(const char *);
//    const char *cc() const;

//    ZString(ZArray<char> bin);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- Assign / Compare:"); // ///////////////////////////////////////////////////////////////////////////////////////////

    TASSERT(str1 != str2);

    str1 = str2;
    LOG(str1);
    TASSERT(str1 == str2);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- Concat / Append:"); // ////////////////////////////////////////////////////////////////////////////////////////////

    ZString str6 = str3.concat(str4);
    LOG(str6);
    TASSERT(str6 == str5);

    str3.append(str4);
    LOG(str3);
    TASSERT(str3 == str5);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- Substr:"); // /////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString substr1 = "youShouldTakeTheRestOfThisString";
    ZString sub1 = ZString::substr(substr1, 16);
    ZString sub1_1 = substr1.substr(16);
    LOG(sub1);
    TASSERT(sub1 == "RestOfThisString" && sub1 == sub1_1);

    ZString substr2 = "youShouldTakeME!InThisString";
    ZString sub2 = ZString::substr(substr2, 13, 3);
    ZString sub2_1 = substr2.substr(13, 3);
    LOG(sub2);
    TASSERT(sub2 == "ME!" && sub2 == sub2_1);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- Insert:"); // /////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString insert1 = "youShouldThisSentence";
    ZString ins1 = ZString::insert(insert1, 9, "Complete");
    ZString ins1_1 = insert1.insert(9, "Complete");
    LOG(ins1);
    TASSERT(ins1 == "youShouldCompleteThisSentence" && ins1 == ins1_1);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- Replace:"); // ////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString replace1 = "anotherInterestingStringWithInterestingThings";
    ZString rep1 = ZString::substitute(replace1, 5, 20, "!!!!!");
    LOG(rep1);
    TASSERT(rep1 == "anoth!!!!!ithInterestingThings");

    replace1.substitute(0, 15, "!!!");
    LOG(replace1);
    TASSERT(replace1 == "!!!ingStringWithInterestingThings");

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- Find:"); // ///////////////////////////////////////////////////////////////////////////////////////////////////////

//    zu64 count(ZString) const;
//    bool startsWith(ZString test, bool ignore_whitespace = true) const;
//    inline bool beginsWith(ZString test) const { return startsWith(test, false); }
//    bool endsWith(ZString test) const;

    ZString find1("someInterestingString");
    zu64 pos1 = find1.findFirst("est");
    zu64 pos1_1 = ZString::findFirst(find1, "est");
    LOG(pos1);
    TASSERT(pos1 == 9 && pos1 == pos1_1);

    ZString find2 = "someInterestingStringIsInteresting";
    zu64 pos2 = ZString::findFirst(find2, "Int", 20);
    LOG(pos2);
    TASSERT(pos2 == 23);

    ZString find3 = "anotherInterestingStringWithInterestingThings";
    ZArray<zu64> pos3 = find3.findAll("est");
    LOG("" << ZLog::NOLN);
    FOREACH(pos3.size()){
        RLOG(pos3[i] << " ");
    }
    RLOG(ZLog::NEWLN);
    TASSERT(pos3.size() == 2 && pos3[0] == 12 && pos3[1] == 33);

    ZString find4 = "anotherInterestesteingStringWithInterestesteingThings";
    ZArray<zu64> pos4 = ZString::findAll(find4, "este");
    LOG("" << ZLog::NOLN);
    FOREACH(pos4.size()){
        RLOG(pos4[i] << " ");
    }
    RLOG(ZLog::NEWLN);
    TASSERT(pos4.size() == 2 && pos4[0] == 12 && pos4[1] == 37);

    ZString find5 = "ssassassb";
    zu64 pos5 = find5.findFirst("ssassb");
    LOG(pos5);
    TASSERT(pos5 == 3);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- Replace:"); // ////////////////////////////////////////////////////////////////////////////////////////////////////

    ZString replace2 = "strposposposposdddddddd";
    ZString rep2 = ZString::replaceRecursive(replace2, "strpos", "bbbstr");
    LOG(rep2);
    TASSERT(rep2 == "bbbbbbbbbbbbstrdddddddd");

    ZString replace3 = "a";
    ZString rep3 = ZString::replaceRecursive(replace3, "a", "bad", 5);
    LOG(rep3);
    TASSERT(rep3 == "bbbbbaddddd");

    replace2.replaceRecursive("strpos", "posstr", 3);
    LOG(replace2);
    TASSERT(replace2 == "posposposstrposdddddddd");

    ZString repalce4 = "strstrsstrstrsstrs";
    ZString rep4 = ZString::replace(repalce4, "strs", "aaaa");
    LOG(rep4);
    TASSERT(rep4 == "aaaatrsaaaatrsaaaa");

    ZString replace5 = "ssssssssssssss";
    ZString rep5 = replace5.replace("ss", "t", 4);
    LOG(rep5);
    TASSERT(rep5 == "ttttssssss");

//    static ZString getUntil(ZString str, ZString find);

//    ZString findFirstBetween(ZString, ZString);
//    ZString replaceBetween(ZString start, ZString end, ZString after);
//    ZString findFirstXmlTagCont(ZString tag);
//    ZString replaceXmlTagCont(ZString tag, ZString after);
//    ZString label(ZString label, ZString value, bool modify = true);
//    ZString label(AsArZ, bool modify = true);

//    ZString &strip(char target);
//    static ZString strip(ZString str, char target);

//    ZString removeWhitespace();

//    ZString &toLower();
//    static ZString toLower(ZString str);

//    ZString duplicate(unsigned iterate, bool modify = true);
//    ZString popLast();
//    static ZString compound(ArZ parts, ZString delim);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- Explode / Compound:"); // /////////////////////////////////////////////////////////////////////////////////////////

//    ArZ split(ZString delim);
//    ArZ explodeList(unsigned nargs, ...);

    ZString strarr = "this!will!!explode!";
    ArZ arr1 = strarr.explode('!');
    ZString cmp1 = ZString::compound(arr1, "-");
    LOG(cmp1);
    TASSERT(arr1.size() == 3 && arr1[0] == "this" && arr1[1] == "will" && arr1[2] == "explode");
    TASSERT(cmp1 == "this-will-explode");

    ZString strarr2 = "this!will!\"sort!of\"!explode\"strstr\"!";
    ArZ arr2 = strarr2.quotedExplode('!');
    ZString cmp2 = ZString::compound(arr2, "-");
    LOG(cmp2);
    TASSERT(arr2.size() == 5 && arr2[0] == "this" && arr2[1] == "will" && arr2[2] == "sort!of" && arr2[3] == "explode" && arr2[4] == "strstr");
    TASSERT(cmp2 == "this-will-sort!of-explode-strstr");

    ZString strarr3 = "\\!\\!!!this!will\\!also!!explode\"strstr\"!";
    ArZ arr3 = strarr3.escapedExplode('!');
    ZString cmp3 = ZString::compound(arr3, "-");
    LOG(cmp3);
    TASSERT(arr3.size() == 4 && arr3[0] == "\\!\\!" && arr3[1] == "this" && arr3[2] == "will\\!also" && arr3[3] == "explode\"strstr\"");
    TASSERT(cmp3 == "\\!\\!-this-will\\!also-explode\"strstr\"");

    ZString strarr4 = "this!.!will!.!explode!.!!.!differently\"strstr\"!.!";
    ArZ arr4 = strarr4.strExplode("!.!");
    ZString cmp4 = ZString::compound(arr4, "---");
    LOG(cmp4);
    TASSERT(arr4.size() == 4 && arr4[0] == "this" && arr4[1] == "will" && arr4[2] == "explode" && arr4[3] == "differently\"strstr\"");
    TASSERT(cmp4 == "this---will---explode---differently\"strstr\"");

    ZString strarr5 = "these!will.all!explode!";
    ArZ arr5 = strarr5.explodeList(2, '!', '.');
    ZString cmp5 = ZString::compound(arr5, "-");
    LOG(cmp5);
    TASSERT(arr5.size() == 4 && arr5[0] == "these" && arr5[1] == "will" && arr5[2] == "all" && arr5[3] == "explode");
    TASSERT(cmp5 == "these-will-all-explode");

//    bool isUtf8(ZString);
//    static bool alphaTest(ZString str1, ZString str2);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- String Iterators:"); // //////////////////////////////////////////////////////////////////////////////////////////

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

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- Number Conversion:"); // //////////////////////////////////////////////////////////////////////////////////////////

//    int tint() const;
//    ZString(double flt, unsigned places = -1);

    ZString itos1 = ZString::ItoS((zu64)4565464, 10);
    LOG(itos1);
    TASSERT(itos1 == "4565464");

    ZString itos2 = ZString::ItoS((zs64)-980948, 16);
    LOG(itos2);
    TASSERT(itos2 == "-ef7d4");

    ZString numstr1 = "3345";
    zu64 num1 = numstr1.tozu64();
    LOG(numstr1 << " " << num1);
    TASSERT(num1 == 3345);

    ZString numstr2 = "546567867864512";
    zu64 num2 = numstr2.tozu64();
    LOG(numstr2 << " " << num2);
    TASSERT(num2 == 546567867864512);

    ZString numstr3 = "FF";
    zu64 num3 = numstr3.tozu64(16);
    LOG(numstr3 << " " << num3);
    TASSERT(num3 == 0xFF);

    ZString numstr4 = "ad562f";
    zu64 num4 = numstr4.tozu64(16);
    LOG(numstr4 << " " << num4);
    TASSERT(num4 == 0xAD562F);

    ZString floatstr3 = "45.223";
    float float3 = floatstr3.toFloat();
    LOG(floatstr3 << " " << float3);

    ZString floatstr4 = "-455";
    float float4 = floatstr4.toFloat();
    LOG(floatstr4 << " " << float4);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- String Formatting:"); // //////////////////////////////////////////////////////////////////////////////////////////
    ZString fmtstr1 = "%s - %s";
    ZString fmt1 = ZString::format(fmtstr1, { "test1", "test2" });
    TASSERT(fmt1 == "test1 - test2");
    LOG(fmt1);

    // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    LOG("-- Unicode:"); // //////////////////////////////////////////////////////////////////////////////////////////
    ZString utf1 = "test";
    utf1.unicode_debug();

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
        LOG('-' << words[i] << "- " << ZLog::NOLN);
    LOG(ZLog::NEWLN);
    //words.concat(t3.strict_explode(' '));
    for(zu64 i = 0; i < words.size(); ++i)
        LOG('-' << words[i] << "- " << ZLog::NOLN);
    LOG(ZLog::NEWLN);

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
    tst3.substitute(3, 5, "ZZZZ");
    LOG(tst3);
    return 0;
}
