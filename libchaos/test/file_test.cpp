#include "test.h"

#include "zfile.h"
#include "zerror.h"

int file_block(){
//    ZPath near = ZString("here");
//    LOG(ZPath::makeDir(near));

    ZPath far = ZString("a/b/c/d/e/f.dat");
    LOG(far.createDirsTo());

    ZFile fl;
    LOG(fl.open(far, ZFile::readwrite | ZFile::createbit));
    LOG(ZError::getSystemError());

    return 0;
}
