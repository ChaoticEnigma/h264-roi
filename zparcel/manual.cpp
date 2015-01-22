#include "parcel-main.h"

ZArray<ZPath> manualFiles(ArZ args){
    ZArray<ZPath> parcelpths;

    if(args.size() < 2){
        LOG("Usage: parcelor manual <file> [... more files ...]");
        exit(EXIT_SUCCESS);
    }
    args.popFront();
    ZPath srcdir = ZPath::pwd();

    LOG("Manual Parcelling Starting..");

    for(zu64 i = 0; i < args.size(); ++i){
        ZPath pth = args[i];
        ZPath ptmpsrc;
        if(pth.absolute())
            ptmpsrc = pth;
        else
            ptmpsrc = srcdir + pth;
        ptmpsrc.sanitize();
        parcelpths.push(ptmpsrc);
    }

    return parcelpths;
}
