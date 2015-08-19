#include "test.h"

#include "zfile.h"
#include "zexception.h"
#include "zrandom.h"
#include "zhash.h"

int file_test(){

    ZPath near = "testdir";
    ZPath farfile = near + "a/b/c/d/e/f.dat";
    ZPath nearfile = near + "near.dat";

    LOG("-- Create Directories:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    LOG("makeDir: " << near);
    if(!ZFile::makeDir(near))
        throw __LINE__;

    LOG("createDirsTo: " << farfile);
    if(!ZFile::createDirsTo(farfile))
        throw __LINE__;

    LOG("-- Write Files:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    {
        ZFile farfl(farfile, ZFile::READWRITE);
        LOG(farfl.isOpen());
        if(farfl.isOpen()){
            LOG(farfl.write("this file is far away"));
            farfl.close();
        } else {
            throw __LINE__;
        }

        ZFile nearfl(nearfile, ZFile::READWRITE);
        LOG(nearfl.isOpen());
        if(nearfl.isOpen()){
            LOG(nearfl.write("this file is nearby"));
            nearfl.close();
        } else {
            throw __LINE__;
        }
    }

    LOG("-- List Files:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZArray<ZPath> nearfiles = ZFile::listFiles(near, false);
    for(zu64 i = 0; i < nearfiles.size(); ++i)
        LOG(nearfiles[i]);

    if(nearfiles.size() != 1)
        throw __LINE__;
    if(nearfiles.front().data() != ArZ({ "testdir","near.dat" }))
        throw __LINE__;

    ZArray<ZPath> farfiles = ZFile::listFiles(near, true);
    for(zu64 i = 0; i < farfiles.size(); ++i)
        LOG(farfiles[i]);

    if(farfiles.size() != 2)
        throw __LINE__;
    zu64 findf = farfiles.find(ZPath("testdir/a/b/c/d/e/f.dat"));
    if(findf == ZArray<ZPath>::none)
        throw __LINE__;
    zu64 finds = farfiles.find(ZPath("testdir/near.dat"));
    if(finds == ZArray<ZPath>::none)
        throw __LINE__;

    LOG("-- Read Files:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    {
        for(zu64 i = 0; i < nearfiles.size(); ++i){
            LOG(nearfiles[i] << " - " << ZFile::readString(nearfiles[i]));
        }

        if(ZFile::readString(nearfiles.front()) != "this file is nearby")
            throw __LINE__;

        for(zu64 i = 0; i < farfiles.size(); ++i){
            LOG(farfiles[i] << " - " << ZFile::readString(farfiles[i]));
        }

        if(ZFile::readString(farfiles[findf]) != "this file is far away")
            throw __LINE__;
        if(ZFile::readString(farfiles[finds]) != "this file is nearby")
            throw __LINE__;
    }

    LOG("-- List Directories:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    {
        ZArray<ZPath> dirs = ZFile::listDirs(near, true);
        for(zu64 i = 0; i < dirs.size(); ++i){
            LOG(dirs[i]);
        }
    }

    LOG("-- Sequential Access:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    {
        ZFile rfile("testrandom", ZFile::READWRITE);
        ZRandom random;
        ZBinary rand1 = random.generate(0x100);
        ZBinary rand2 = random.generate(0x100);
        ZBinary rand3 = random.generate(0x100);
        ZBinary rand4 = random.generate(0x100);
        rfile.write(rand1);
        rfile.write(rand2);
        rfile.write(rand3);
        rfile.write(rand4);
        rfile.rewind();
        ZBinary randi;
        rfile.read(randi, 0x100);
        if(ZHash<ZBinary>(randi).hash() != ZHash<ZBinary>(rand1).hash())
            throw __LINE__;
        rfile.read(randi, 0x100);
        if(ZHash<ZBinary>(randi).hash() != ZHash<ZBinary>(rand2).hash())
            throw __LINE__;
        rfile.read(randi, 0x100);
        if(ZHash<ZBinary>(randi).hash() != ZHash<ZBinary>(rand3).hash())
            throw __LINE__;
        rfile.read(randi, 0x100);
        if(ZHash<ZBinary>(randi).hash() != ZHash<ZBinary>(rand4).hash())
            throw __LINE__;
    }

    return 0;
}
