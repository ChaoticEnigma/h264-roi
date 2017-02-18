#include "zlog.h"
#include "zfile.h"
#include "zassoc.h"
#include "zbinary.h"
#include "zhash.h"
using namespace LibChaos;

// Find duplicate files

int main(int argc, char **argv){
    ZLog::logLevelStdOut(ZLog::INFO, "%log%");

    ArZ param_args;
    for(int i = 0; i < argc; ++i){
        param_args.push(argv[i]);
    }
    param_args.popFront();

    ZPath search = ZPath::pwd();
    if(param_args.size() == 1){
        search = param_args[0];
    }

    LOG("Looking for duplicates");
    zu64 count = 0;
    ZArray<ZPath> files = ZFile::listFiles(search);
    LOG("Testing " << files.size() << " files");

    ZAssoc<zu64, ZArray<ZPath>> hashtable;
    for(zu64 i = 0; i < files.size(); ++i){
        ZPath path = files[i];
        ZBinary bin;
        ZFile::readBinary(path, bin);
        //ZFile(path).read(bin);
        zu64 hash = ZHash<ZBinary>(bin).hash();
        //LOG(hash << " - " << bin.size() << " - " << path);
        hashtable[hash].push(path);
        if(i % 100 == 0){
            LOG(i);
        }
    }
    for(zu64 i = 0; i < hashtable.size(); ++i){
        zu64 key = hashtable.key(i);
        if(hashtable[key].size() > 1){
            ZString list;
            for(zu64 j = 0; j < hashtable[key].size(); ++j){
                list << '"' << hashtable[key][j].str() << "\", ";
            }
            LOG(key << " - " << list);
            ++count;
        }
    }
    LOG("Finished: " << count << " duplicate hashes");
    return 0;
}

