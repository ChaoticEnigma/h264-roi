#include "test.h"

int path_block(){
    LOG("-- Parse:");  // /////////////////////////////////////////////////////////////////////////////////////////////////////////

    ZPath path1 = ZString("/some/odd/../complic/ated/../path");
    LOG(path1.prefix());
    LOG(path1.absolute());
    LOG(path1);

    LOG("-- Sanitize:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    path1.sanitize();
    LOG(path1);
    if(path1.size() != 3 || !path1.absolute() || path1[0] != "some" || path1[1] != "complic" || path1[2] != "path")
        throw 1;

    ZPath path2 = "../../another/path";
    path2.sanitize();
    LOG(path2);
    if(path2.size() != 4 || path2.absolute() || path2[0] != ".." || path2[1] != ".." || path2[2] != "another" || path2[3] != "path")
        throw 2;

    ZPath path3 = "/some/../../../path";
    path3.sanitize();
    LOG(path3);
    if(path3.size() != 3 || !path3.absolute() || path3[0] != ".." || path3[1] != ".." || path3[2] != "path")
        throw 3;

    LOG("-- Relative To:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    // Examples

    // /a/path/here/test.txt
    // /this/path/to/another/place
    // ../../../../../a/path/here/test.txt

    // /some/short/thing.obj
    // /some/short/other/path
    // ../../thing.obj

    ZPath path4 = "/some/odd/../complic/ated/../path";
    ZPath path5 = "/";
    LOG(path4);
    LOG(path5);
    path4.relativeTo(path5);
    LOG(path4);
    if(path4.size() != 3 || path4.absolute() || path4[0] != "some" || path4[1] != "complic" || path4[2] != "path")
        throw 4;

    ZPath path6 = "/another/long/interesting/path";
    ZPath path7 = "/another/short/boring/path";
    LOG(path6);
    LOG(path7);
    path7.relativeTo(path6);
    LOG(path6);
    if(path6.size() != 6 || path6.absolute() || path6[0] != ".." || path6[1] != ".." || path6[2] != ".." || path6[3] != "short" || path6[4] != "boring" || path6[5] != "path")
        throw 5;

    return 0;
}
