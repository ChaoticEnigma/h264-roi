#include "test.h"

#include "zfile.h"
#include "zerror.h"

int file_block(){
    ZPath near = ZString("here");
    LOG("makeDir: " << near);
    if(!ZFile::makeDir(near))
        throw 1;

    ZPath farfile = "a/b/c/d/e/f.dat";
    LOG("createDirsTo: " << farfile);
    if(!ZFile::createDirsTo(farfile))
        throw 2;

    ZArray<ZPath> farfls = ZFile::listFiles("a");
    if(farfls.isEmpty())
        throw 3;
    for(zu64 i = 0; i < farfls.size(); ++i){
        LOG(farfls[i]);
    }

    ZFile fl(farfile, ZFile::modereadwrite);
    LOG(fl.isOpen());
    if(fl.isOpen()){
        LOG(fl.write("this is a file"));
        fl.close();
    } else {
        throw 4;
    }

    return 0;
}
