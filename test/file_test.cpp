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
    TASSERT(ZFile::makeDir(near));

    LOG("createDirsTo: " << farfile);
    TASSERT(ZFile::createDirsTo(farfile));

    LOG("-- Write Files:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    {
        ZFile farfl(farfile, ZFile::READWRITE);
        LOG(farfl.isOpen());
        TASSERT(farfl.isOpen());
        LOG(farfl.write("this file is far away"));
        farfl.close();

        ZFile nearfl(nearfile, ZFile::READWRITE);
        LOG(nearfl.isOpen());
        TASSERT(nearfl.isOpen());
        LOG(nearfl.write("this file is nearby"));
        nearfl.close();
    }

    LOG("-- List Files:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    ZArray<ZPath> nearfiles = ZFile::listFiles(near, false);
    for(zu64 i = 0; i < nearfiles.size(); ++i)
        LOG(nearfiles[i]);

    TASSERT(nearfiles.size() == 1);
    TASSERT(nearfiles.front().data() == ArZ({ "testdir", "near.dat" }))

    ZArray<ZPath> farfiles = ZFile::listFiles(near, true);
    for(zu64 i = 0; i < farfiles.size(); ++i)
        LOG(farfiles[i]);

    TASSERT(farfiles.size() == 2);
    zu64 findf = farfiles.find(ZPath("testdir/a/b/c/d/e/f.dat"));
    TASSERT(findf != ZArray<ZPath>::none);
    zu64 finds = farfiles.find(ZPath("testdir/near.dat"));
    TASSERT(finds != ZArray<ZPath>::none);

    LOG("-- Read Files:");  // //////////////////////////////////////////////////////////////////////////////////////////////////////

    {
        for(zu64 i = 0; i < nearfiles.size(); ++i){
            LOG(nearfiles[i] << " - " << ZFile::readString(nearfiles[i]));
        }

        TASSERT(ZFile::readString(nearfiles.front()) == "this file is nearby");

        for(zu64 i = 0; i < farfiles.size(); ++i){
            LOG(farfiles[i] << " - " << ZFile::readString(farfiles[i]));
        }

        TASSERT(ZFile::readString(farfiles[findf]) == "this file is far away");
        TASSERT(ZFile::readString(farfiles[finds]) == "this file is nearby");
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
        TASSERT(ZHash<ZBinary>(randi).hash() == ZHash<ZBinary>(rand1).hash());
        rfile.read(randi, 0x100);
        TASSERT(ZHash<ZBinary>(randi).hash() == ZHash<ZBinary>(rand2).hash());
        rfile.read(randi, 0x100);
        TASSERT(ZHash<ZBinary>(randi).hash() == ZHash<ZBinary>(rand3).hash());
        rfile.read(randi, 0x100);
        TASSERT(ZHash<ZBinary>(randi).hash() == ZHash<ZBinary>(rand4).hash());
    }

    return 0;
}
