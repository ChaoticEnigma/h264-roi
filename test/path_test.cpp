#include "test.h"

int path_test(){
    LOG("-- Parse:");  // /////////////////////////////////////////////////////////////////////////////////////////////////////////

    ZPath path1 = ZString("/some/odd/../complic/ated/../path");
    LOG(path1.prefix());
    LOG(path1.absolute());
    LOG(path1);

    LOG("-- Sanitize:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    path1.sanitize();
    LOG(path1);
    if(path1.data() != ArZ({ "some","complic","path" }) || !path1.absolute())
        throw 1;

    ZPath path2 = "../../another/path";
    path2.sanitize();
    LOG(path2);
    if(path2.data() != ArZ({ "..","..","another","path" }) || path2.absolute())
        throw 2;

    ZPath path3 = "/some/../../../path";
    path3.sanitize();
    LOG(path3);
    if(path3.data() != ArZ({ "..","..","path" }) || !path3.absolute())
        throw 3;

    LOG("-- Relative To:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZPath path4 = "/some/odd/../complic/ated/../path";
    ZPath path5 = "/";
    LOG(path4);
    LOG(path5);
    path4.relativeTo(path5);
    LOG(path4);
    if(path4.data() != ArZ({ "some","complic","path" }) || path4.absolute())
        throw 4;

    ZPath path6 = "/another/long/interesting/path/to/nowhere";
    ZPath path7 = "/another/short/boring/path";
    LOG(path6);
    LOG(path7);
    path6.relativeTo(path7);
    LOG(path6);
    if(path6.data() != ArZ({ "..","..","..","long","interesting","path","to","nowhere" }) || path6.absolute())
        throw 5;

    // /a/path/here/test.txt
    // /this/path/to/another/place
    // ../../../../../a/path/here/test.txt

    ZPath path8 = "/a/path/here/test.txt";
    ZPath path9 = "/this/path/to/another/place";
    LOG(path8);
    LOG(path9);
    path8.relativeTo(path9);
    LOG(path8);
    if(path8.data() != ArZ({ "..","..","..","..","..","a","path","here","test.txt" }) || path8.absolute())
        throw 6;

    // /some/short/thing.obj
    // /some/short/other/path
    // ../../thing.obj

    ZPath path10 = "/some/short/thing.obj";
    ZPath path11 = "/some/short/other/path";
    LOG(path10);
    LOG(path11);
    path10.relativeTo(path11);
    LOG(path10);
    if(path10.data() != ArZ({ "..","..","thing.obj" }) || path10.absolute())
        throw 7;

    return 0;
}
