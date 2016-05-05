#include "tests.h"

namespace LibChaosTest {

void path_parse(){
    ZPath path1 = ZString("/some/odd/../complic/ated/../path");
    LOG(path1.prefix());
    LOG(path1.absolute());
    LOG(path1);
}

void path_sanitize(){
    ZPath path1 = ZString("/some/odd/../complic/ated/../path");
    path1.sanitize();
    LOG(path1);
    TASSERT(path1.data() == ArZ({ "some","complic","path" }) && path1.absolute());

    ZPath path2 = "../../another/path";
    path2.sanitize();
    LOG(path2);
    TASSERT(path2.data() == ArZ({ "..","..","another","path" }) && !path2.absolute());

    ZPath path3 = "/some/../../../path";
    path3.sanitize();
    LOG(path3);
    TASSERT(path3.data() == ArZ({ "..","..","path" }) && path3.absolute());
}

void path_relative(){
    ZPath path4 = "/some/odd/../complic/ated/../path";
    ZPath path5 = "/";
    LOG(path4);
    LOG(path5);
    path4.relativeTo(path5);
    LOG(path4);
    TASSERT(path4.data() == ArZ({ "some","complic","path" }) && !path4.absolute());

    ZPath path6 = "/another/long/interesting/path/to/nowhere";
    ZPath path7 = "/another/short/boring/path";
    LOG(path6);
    LOG(path7);
    path6.relativeTo(path7);
    LOG(path6);
    TASSERT(path6.data() == ArZ({ "..","..","..","long","interesting","path","to","nowhere" }) && !path6.absolute());

    // /a/path/here/test.txt
    // /this/path/to/another/place
    // ../../../../../a/path/here/test.txt

    ZPath path8 = "/a/path/here/test.txt";
    ZPath path9 = "/this/path/to/another/place";
    LOG(path8);
    LOG(path9);
    path8.relativeTo(path9);
    LOG(path8);
    TASSERT(path8.data() == ArZ({ "..","..","..","..","..","a","path","here","test.txt" }) && !path8.absolute());

    // /some/short/thing.obj
    // /some/short/other/path
    // ../../thing.obj

    ZPath path10 = "/some/short/thing.obj";
    ZPath path11 = "/some/short/other/path";
    LOG(path10);
    LOG(path11);
    path10.relativeTo(path11);
    LOG(path10);
    TASSERT(path10.data() == ArZ({ "..","..","thing.obj" }) && !path10.absolute());
}

ZArray<Test> path_tests(){
    return {
        { "path-parse",     path_parse,     true, { "string-explode-compound" } },
        { "path-sanitize",  path_sanitize,  true, { "array-equality", "path-parse" } },
        { "path-relative",  path_relative,  true, { "array-equality", "path-parse" } },
    };
}

}
