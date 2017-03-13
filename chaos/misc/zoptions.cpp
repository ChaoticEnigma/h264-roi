#include "zoptions.h"
#include "zlog.h"

namespace LibChaos {

ZOptions::ZOptions(ZArray<OptDef> optdef){
    define = optdef;
}

bool ZOptions::parse(int argc, char **argv){
    bool nextarg = false;
    ZString nextname;

    for(int i = 1; i < argc; ++i){
        ZString arg = argv[i];
        if(arg.beginsWith("--") && arg.size() > 2){
            // Long option
            arg.substr(2);
            bool ok = false;
            for(zu64 j = 0; j < define.size(); ++j){
                ZString pref = define[j].name + "=";
                if(arg == define[j].name){
                    if(define[j].type == NONE){
                        opts[define[j].name] = "";
                    } else {
                        nextname = define[j].name;
                        nextarg = true;
                    }
                    ok = true;
                    break;
                } else if(arg.beginsWith(pref)){
                    arg.substr(pref.size());
                    opts[define[j].name] = arg;
                    ok = true;
                }
            }
            if(!ok){
                LOG("error: unknown long option: " << arg);
                return false;
            }

        } else if(arg.beginsWith("-") && arg.size() > 1){
            // Flag option
            arg.substr(1);
            bool ok = false;
            bool noarg = true;
            // multiple flags possible
            for(zu64 k = 0; noarg && k < arg.size(); ++k){
                // check options
                for(zu64 j = 0; j < define.size(); ++j){
                    if(arg[k] == define[j].flag){
                        if(define[j].type == NONE){
                            opts[define[j].name] = "";
                        } else {
                            noarg = false;
                            arg.substr(k+1);
                            if(arg.isEmpty()){
                                nextname = define[j].name;
                                nextarg = true;
                            } else {
                                opts[define[j].name] = arg;
                            }
                        }
                        ok = true;
                        break;
                    }
                }
                if(!ok){
                    LOG("error: unknown flag option: " << arg);
                    return false;
                }
            }

        } else if(nextarg){
            // Option argument
            opts[nextname] = arg;
            nextarg = false;

        } else {
            // Normal arg
            args.push(arg);
        }
    }

    if(nextarg){
        LOG("error: no value for option: " << nextname);
        return false;
    }
    return true;
}

}
