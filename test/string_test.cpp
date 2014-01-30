#include "test.h"

#include "zstring.h"
#include "zpath.h"

int string_block(){
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

    ZPath mk = "over/there/lol.txt";
    LOG(mk);
    if(mk.createDirsTo())
        LOG("ok");
    else
        LOG("no");
    return 0;
}
