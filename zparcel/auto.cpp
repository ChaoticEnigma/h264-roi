#include "parcel-main.h"

ZArray<ZPath> autoFiles(ArZ args){
    ZArray<ZPath> parcelpths;

    if(args.size() < 2){
        LOG("Usage: parcelor auto <source_dir> [... more source dirs ...]");
        exit(EXIT_SUCCESS);
    }
    args.popFront();

    ZArray<ZPath> parcelsrcs;
    for(zu64 i = 0; i < args.size(); ++i){
        ZPath ptmpsrc = args[i];
        ptmpsrc.getAbsolute();
        parcelsrcs.push(ptmpsrc);
    }

    LOG("Automatically Finding and Parcelling Files from:");
    for(zu64 i = 0; i < parcelsrcs.size(); ++i){
        LOG("\"" << parcelsrcs[i] << "\"");
    }

    unsigned excld = 0;
    for(zu64 i = 0; i < parcelsrcs.size(); ++i){
        ZPath ptmpsrc = parcelsrcs[i];
        ZArray<ZPath> tmplist = ZFile::listFiles(ptmpsrc);

        ZArray<ZPath> exclude;
        for(zu64 j = 0; j < tmplist.size(); ++j){
            ZPath ptmpsrcfl = tmplist[j];
            if(ptmpsrcfl.last() == "parcelexclude"){
                LOG("Parcel Exclusion File: \"" << ptmpsrcfl.str() << "\" Found");
                ZBinary setdat;
                ZFile::readBinary(ptmpsrcfl.str(), setdat);
                ZString sett = setdat.printable().asChar();
                if(!ZJSON::validJSON(sett)){
                    LOG("! Invalid JSON in Parcel Exclusion File: \"" << ptmpsrcfl.str() << "\", Ignoring File");
                    continue;
                }
                ZJSON settings(sett);
                ZPath contdir = ptmpsrcfl.parent();
                ZString flslst = settings["files"];
                if(!flslst.isEmpty()){
                    if(!ZJSON::validJSON(flslst)){
                        LOG("! Invalid JSON in \"files\" section of Parcel Exclusion File: \"" << ptmpsrcfl.str() << "\", Ignoring File");
                        continue;
                    }
                    ZJSON exclfls(flslst);
                    for(zu64 k = 0; k < exclfls.size(); ++k){
                        if(exclfls.key(k) == "exclude"){
                            ZPath tmpexfl = contdir;
                            tmpexfl.concat(ZPath(exclfls[k]));
                            exclude.push(tmpexfl);
                        }
                    }
                }
                //ZString settlst = settings["settings"];
                // Do stuff with settings...
            }
        }

        for(zu64 j = 0; j < tmplist.size(); ++j){
            ZPath ptmpsrcfl = tmplist[j];
            bool exclfl = false;
            for(zu64 k = 0; k < exclude.size(); ++k){
                if(ptmpsrcfl.childTo(exclude[k])){
                    ++excld;
                    exclfl = true;
                    break;
                }
            }
            if(!exclfl && ptmpsrcfl.last() != "parcelexclude"){
                parcelpths.push(ptmpsrcfl);
            }
            //++total;
        }
    }
    if(excld > 0)
        LOG("Excluding " << excld << " Files as Set by parcelexclude File(s)");

    return parcelpths;
}
