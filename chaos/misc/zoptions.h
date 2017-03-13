#ifndef ZOPTIONS_H
#define ZOPTIONS_H

#include "zstring.h"
#include "zmap.h"

namespace LibChaos {

class ZOptions {
public:
    enum OptType {
        NONE,
        STRING,
        INTEGER,
    };

    struct OptDef {
        ZString name;
        char flag;
        OptType type;
    };

public:
    ZOptions(ZArray<OptDef> optdef);

    bool parse(int argc, char ** argv);

    ZArray<ZString> getArgs() const {
        return args;
    }
    ZMap<ZString, ZString> getOpts(){
        return opts;
    }

private:
    ZArray<OptDef> define;

    ZArray<ZString> args;
    ZMap<ZString, ZString> opts;
};

}

#endif // ZOPTIONS_H
