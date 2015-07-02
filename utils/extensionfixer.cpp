#include "zlog.h"
#include "zfile.h"
#include "zbinary.h"
using namespace LibChaos;

// Rename files starting with JPEG magic numbers to .jpg

ZPath changeExtension(ZPath path, ZString ext){
    ArZ parts = path.last().explode('.');
    if(parts.size() > 1)
        parts.popBack();
    path.last() = ZString::compound(parts, '.') << "." << ext;
    return path;
}

int main(){
    LOG("Fixing image file extensions...");
    zu64 count = 0;
    ZArray<ZPath> files = ZFile::listFiles(".", false);
    LOG("Testing " << files.size() << " files");
    for(zu64 i = 0; i < files.size(); ++i){
        ZPath path = files[i];
        LOG(path);
        ZBinary bin;
        ZFile(path).read(bin, 2);
        if(bin[0] == (zu8)0xFF && bin[1] == (zu8)0xD8){ // JPEG
            ZPath tmppath = changeExtension(path, "jpg");
            ZFile::copy(path, tmppath);
            if(ZFile::remove(path)){
                LOG("could not remove " << path.str());
            }
            LOG("JPEG file " << path << " moved to " << tmppath);
            ++count;
        }
    }
    LOG("Finished: renamed " << count << " files.");
    return 0;
}

