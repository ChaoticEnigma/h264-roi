#include "parcel-main.h"
#include "zerror.h"

int mainwrap(int argc, char **argv){
    ZLog::formatStdout(ZLogSource::normal, "%log%");
    ZLog::formatStdout(ZLogSource::debug, "%time% (%file%:%line%) - %log%");
    ZLog::formatStderr(ZLogSource::error, "%time% (%file%:%line%) - %log%");

    // Arguments
    ZString cmdstr;
    ArZ args;
    AsArZ flags;
    for(int i = 0; i < argc; ++i){
        cmdstr << argv[i] << " ";
        if(argv[i][0] == '-'){
            ZString tmp = argv[i];
            tmp.substr(1);
            flags.push(tmp);
        } else {
            args.push(argv[i]);
        }
    }
    args.popFront();
    cmdstr.substr(0, cmdstr.size() - 1);

    LOG("Parcelor Command: \"" << cmdstr << "\"");

    // Select mode and get file list
    ZArray<ZPath> parcelpths;
    if(args.size() > 0 && args[0] == "auto"){
        // Auto parcel directory
        parcelpths = autoFiles(args);
    } else if(args.size() > 0 && args[0] == "manual"){
        // Manual parcel files
        parcelpths = manualFiles(args);
    } else if(args.size() > 0 && args[0] == "create"){
        // Create empty parcel

    } else if(args.size() > 0 && args[0] == "list"){
        // List parcel contents
        if(args.size() > 1){
            ZParcel parcel;
            parcel.open(args[1]);
            for(zu64 i = 0; i < parcel.getIndex().size(); ++i){
                LOG(parcel.getIndex().key(i) << " " << parcel.getIndex()[i].pos << " " << parcel.getIndex()[i].len);
            }
        } else {
            LOG("No parcel file specified");
        }
        return EXIT_SUCCESS;
    } else if(args.size() > 0 && args[0] == "unpack"){
        // Extract parcel contents
        if(args.size() > 1){
            ZParcel parcel;
            parcel.open(args[1]);
            for(zu64 i = 0; i < parcel.getIndex().size(); ++i){
                LOG(parcel.getIndex().key(i) << " " << parcel.getIndex()[i].pos << " " << parcel.getIndex()[i].len);
            }
        } else {
            LOG("No parcel file specified");
        }
        return EXIT_SUCCESS;
    } else {
        LOG("Usage:");
        LOG("    zparcel auto <source_dir> [... more source dirs  ...]");
        LOG("    zparcel manual <file> [... more files ...]");
        LOG("    zparcel create <new_parcel>");
        LOG("    zparcel list <parcel>");
        LOG("    zparcel unpack <parcel>");
        return EXIT_SUCCESS;
    }

    // Settings
    ZPath pwd = ZPath::pwd();
    ZPath parcelname = "parcel.zparcel";
    ZPath reldir = pwd;

    // Get flag settings
    for(zu64 i = 0; i < flags.size(); ++i){
        if(flags[i].startsWith("-output=", false)){
            parcelname = flags[i].substr(8);
        } else if(flags[i].startsWith("-relative=", false)){
            reldir = flags[i].substr(10);
        } else {
            LOG("Unknown Flag: " << flags[i]);
        }
    }

    // Resolve parcel location
    if(!parcelname.absolute())
        parcelname = pwd + parcelname;

    // Create section list
    ZAssoc<ZString, ZPath> parcelfls;
    for(zu64 i = 0; i < parcelpths.size(); ++i){
        ZPath pth = parcelpths[i];
        pth.relativeTo(reldir);
        //LOG(parcelpths[i] << " --> " << pth);
        parcelfls[pth.str('/')] = parcelpths[i];
    }

    // Sort files
    ZAssoc<ZString, ZPath> tmppcfls;
    for(zu64 i = 0; i < parcelfls.size(); ++i){
        ZString key = parcelfls.key(i);
        ZPath val = parcelfls[i];
        bool found = false;
        for(zu64 j = 0; j < tmppcfls.size(); ++j){
            ZString old = tmppcfls.key(j);
            if(ZString::alphaTest(key, old)){
                tmppcfls.insert(j, key, val);
                found = true;
                break;
            }
        }
        if(!found){
            tmppcfls.push(key, val);
        }
    }
    parcelfls = tmppcfls;

    LOG("Parcelling " << parcelfls.size() << " Files:");
    LOG("Naming Parcel Contents Relative to: " << reldir);

    ZFile::remove(parcelname);

    // Create multi-step parcel
    ZParcel outparcel;
    if(!outparcel.newParcel(parcelname)){
        LOG("Could not make new parcel");
        return EXIT_FAILURE;
    }

    ZAssoc<ZString, zu64> hashes;

    // Add first integrity block
    ZBinary integrity = ZBinary("\xFF\x00ZEPHYR INTEGRITY TEST FRONT\x00\xFF", 31);
    if(!outparcel.addData("integrity-front.dat", integrity)){
        LOG("Could not add data to parcel");
        return EXIT_FAILURE;
    }
    hashes.push("integrity-front.dat", ZBinary::hash(ZBinary::hashType1, integrity));

    // Add files to parcel
    for(zu64 i = 0; i < parcelfls.size(); ++i){
//        ZBinary dat;
//        ZFile::readBinary(parcelfls[i], dat);
//        if(!outparcel.addData(parcelfls.key(i), dat)){
//            LOG("Could not add file to parcel");
//            return EXIT_FAILURE;
//        }
        hashes.push(parcelfls.key(i), ZFile::fileHash(parcelfls[i]));
        if(!outparcel.addFile(parcelfls.key(i), parcelfls[i])){
            LOG("Could not add file to parcel");
            return EXIT_FAILURE;
        }
    }

    // Add last integrity block
    integrity = ZBinary("\xFF\x00ZEPHYR INTEGRITY TEST BACK\x00\xFF", 30);
    if(!outparcel.addData("integrity-back.dat", integrity)){
        LOG("Could not add data to parcel");
        return EXIT_FAILURE;
    }
    hashes.push("integrity-back.dat", ZBinary::hash(ZBinary::hashType1, integrity));

    zu64 writebytes = outparcel.finishParcel();

    if(writebytes > 0){
        LOG("Parcel: " << parcelname);
        LOG("Parcelled Bytes: " << writebytes);

        ZParcel parcel;
        if(!parcel.open(parcelname)){
            ELOG("Parcel " << parcelname << " failed to open");
            return EXIT_FAILURE;
        }

        zu64 contsize = parcel.parcelContSize();
        LOG("Section Bytes: " << contsize);
        if(contsize != writebytes){
            ELOG("Parcelled Size Mismatch!");
        }

        zu64 unparcelsize = parcel.unParcel();
        LOG("UnParcel Bytes: " << unparcelsize);
        if(unparcelsize != writebytes){
            ELOG("UnParcelled Size Mismatch!");
        }

        ZPath base = parcel.parcelFile();
        base.last().replace(".", "_").append("_cont");
        base.getAbsolute();

        zu64 pass = 0;
        for(zu64 i = 0; i < hashes.size(); ++i){
//            ZBinary data;
//            if(!parcel.getByName(hashes.key(i), data)){
//                ELOG("Failed to get section " << hashes.key(i) << " for hash " << data.size());
//                continue;
//            }

            zu64 hash = ZFile::fileHash(base + hashes.key(i));
            //LOG(i << " : " << hash << " - " << hashes[i] << " : " << hashes.key(i));
            if(hash == hashes[i]){
                ++pass;
            } else {
                ELOG("Hash fail on " << hashes.key(i) << " " << hash);
            }
        }

        if(pass == hashes.size())
            LOG("All Hashes passed");
        else
            LOG(pass << "/" << hashes.size() << " Hashes passed");

    } else {
        LOG("Parcel Building Failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv){
    try {
        return mainwrap(argc, argv);
    } catch(ZError err){
        ELOG(err.what());
        return EXIT_FAILURE;
    }
}
