#include "test.h"

#include "zfile.h"
#include "zerror.h"

int file_block(){

    LOG("-- Create Directories:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZPath near = ZString("testdir");
    LOG("makeDir: " << near);
    if(!ZFile::makeDir(near))
        throw 1;

    ZPath farfile = near + "a/b/c/d/e/f.dat";
    LOG("createDirsTo: " << farfile);
    if(!ZFile::createDirsTo(farfile))
        throw 2;

    LOG("-- Write Files:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZFile farfl(farfile, ZFile::modereadwrite);
    LOG(farfl.isOpen());
    if(farfl.isOpen()){
        LOG(farfl.write("this file is far away"));
        farfl.close();
    } else {
        throw 4;
    }

    ZPath nearfile = near + "near.dat";
    ZFile nearfl(nearfile, ZFile::modereadwrite);
    LOG(nearfl.isOpen());
    if(nearfl.isOpen()){
        LOG(nearfl.write("this file is nearby"));
        nearfl.close();
    } else {
        throw 5;
    }

    LOG("-- List Files:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZArray<ZPath> nearfiles = ZFile::listFiles(near, false);
    for(zu64 i = 0; i < nearfiles.size(); ++i)
        LOG(nearfiles[i]);

    if(nearfiles.size() != 1)
        throw 6;
    if(nearfiles.front().data() != ArZ({ "testdir","near.dat" }))
        throw 7;

    ZArray<ZPath> farfiles = ZFile::listFiles(near, true);
    for(zu64 i = 0; i < farfiles.size(); ++i)
        LOG(farfiles[i]);

    if(farfiles.size() != 2)
        throw 8;
    zu64 findf = farfiles.find(ZPath("testdir/a/b/c/d/e/f.dat"));
    if(findf == ZArray<ZPath>::none)
        throw 9;
    zu64 finds = farfiles.find(ZPath("testdir/near.dat"));
    if(finds == ZArray<ZPath>::none)
        throw 10;

    LOG("-- Read Files:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    for(zu64 i = 0; i < nearfiles.size(); ++i){
        LOG(nearfiles[i] << " - " << ZFile::readString(nearfiles[i]));
    }

    if(ZFile::readString(nearfiles.front()) != "this file is nearby")
        throw 11;

    for(zu64 i = 0; i < farfiles.size(); ++i){
        LOG(farfiles[i] << " - " << ZFile::readString(farfiles[i]));
    }

    if(ZFile::readString(farfiles[findf]) != "this file is far away")
        throw 12;
    if(ZFile::readString(farfiles[finds]) != "this file is nearby")
        throw 13;

    LOG("-- List Directories:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZArray<ZPath> dirs = ZFile::listDirs(near, true);
    for(zu64 i = 0; i < dirs.size(); ++i){
        LOG(dirs[i]);
    }

    return 0;
}
