#include "test.h"

#include "zfile.h"
#include "zerror.h"

int file_block(){
    ZPath near = ZString("here");
    LOG("makeDir: " << near);
    if(!ZFile::makeDir(near))
        throw (int)1;

    ZPath farfile = "a/b/c/d/e/f.dat";
    LOG("createDirsTo: " << farfile);
    if(!ZFile::createDirsTo(farfile))
        throw (int)2;

    ZArray<ZPath> farfl = ZFile::listFiles("a");
    for(zu64 i = 0; i < farfl.size(); ++i)
        LOG(farfl[i]);

    ZFile fl;
    LOG(fl.open(farfile, ZFile::modereadwrite));
    LOG(ZError::getSystemError());

    return 0;
}
