#include "test.h"

#include "zfile.h"
#include "zerror.h"

int file_block(){
    ZPath near = ZString("here");
    LOG(ZFile::makeDir(near));

    ZPath farfile = "a/b/c/d/e/f.dat";
    LOG(ZFile::createDirsTo(farfile));

    ZArray<ZPath> farfl = ZFile::listFiles("a");
    for(zu64 i = 0; i < farfl.size(); ++i)
        LOG(farfl[i]);

    ZFile fl;
    LOG(fl.open(farfile, ZFile::modereadwrite));
    LOG(ZError::getSystemError());

    return 0;
}
