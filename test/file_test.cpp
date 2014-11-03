#include "test.h"

#include "zfile.h"
#include "zerror.h"

int file_block(){
//    ZPath near = ZString("here");
//    LOG(ZPath::makeDir(near));

    ZPath farfile = ZString("a/b/c/d/e/f.dat");
    LOG(ZFile::createDirsTo(farfile));

    ZFile fl;
    LOG(fl.open(farfile, ZFile::modereadwrite));
    LOG(ZError::getSystemError());

    return 0;
}
