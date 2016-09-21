#include "main.h"
#include "zexception.h"
#include "zlog.h"

using namespace LibChaos;

static const ZMap<ZString, ZParcel::objtype> nametotype= {
    { "null",   ZParcel::NULLOBJ },
    { "uint",   ZParcel::UINTOBJ },
    { "sint",   ZParcel::SINTOBJ },
    { "float",  ZParcel::FLOATOBJ },
    { "uid",    ZParcel::ZUIDOBJ },
    { "zuid",   ZParcel::ZUIDOBJ },
    { "str",    ZParcel::STRINGOBJ },
    { "string", ZParcel::STRINGOBJ },
    { "file",   ZParcel::FILEOBJ },
    { "blob",   ZParcel::BLOBOBJ },
    { "binary", ZParcel::BLOBOBJ },
};

int mainwrap(int argc, char **argv){
    ZLog::logLevelStdOut(ZLog::INFO, "%log%");
    ZLog::logLevelStdOut(ZLog::DEBUG, "%time% (%file%:%line%) - %log%");
    ZLog::logLevelStdErr(ZLog::ERRORS, "%time% (%file%:%line%) - %log%");

    // Arguments
    ZString cmdstr;
    ZString argstr;
    ArZ args;
    AsArZ flags;
    for(int i = 0; i < argc; ++i){
        cmdstr << argv[i] << " ";
        if(i > 0) argstr << argv[i] << " ";
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
    argstr.substr(0, argstr.size() - 1);

    LOG("ZParcel Command: \"" << argstr << "\"");

    if(args.size() > 0 && args[0] == "create"){
        if(args.size() != 2){
            LOG("Usage: create <file>");
            return EXIT_FAILURE;
        }
        LOG("Creating new ZParcel " << args[1]);

        ZParcel parcel;
        bool ok = parcel.create(ZString(args[1]));
        LOG("Create: " << ok);

    } else if(args.size() > 0 && args[0] == "list"){
        if(args.size() != 2){
            LOG("Usage: list <file>");
            return EXIT_FAILURE;
        }
        LOG("Listing Objects in ZParcel " << args[1]);

    } else if(args.size() > 0 && args[0] == "store"){
        if(args.size() != 5){
            LOG("Usage: store <file> <id> <type> <value>");
            return EXIT_FAILURE;
        }
        LOG("Storing Object in ZParcel " << args[1]);

        ZParcel parcel;
        bool ok = parcel.open(ZString(args[1]));
        if(ok){
            LOG(args[2]);
            ZString type = args[3];
            if(type == "null"){
                parcel.storeNull(ZUID(args[2]));
            } else if(type == "bool"){
                parcel.storeBool(ZUID(args[2]), (ZString(args[4]) == "true" ? true : false));
            } else if(type == "uint"){
                parcel.storeUint(ZUID(args[2]), ZString(args[4]).tozu64());
            } else if(type == "int" ||
                      type == "sint"){
                parcel.storeSint(ZUID(args[2]), ZString(args[4]).tint());
            } else if(type == "float" ||
                      type == "double"){
                parcel.storeFloat(ZUID(args[2]), ZString(args[4]).toFloat());
            } else if(type == "uid" ||
                      type == "uuid" ||
                      type == "zuid"){
                parcel.storeZUID(ZUID(args[2]), ZUID(args[4]));
            } else if(type == "str" ||
                      type == "string"){
                parcel.storeString(ZUID(args[2]), ZString(args[4]));
            } else if(type == "bin" ||
                      type == "blob" ||
                      type == "binary"){
                ZBinary bin;
                ZFile::readBinary(ZString(args[4]), bin);
                parcel.storeBlob(ZUID(args[2]), bin);
            } else if(type == "file"){
                parcel.storeFile(ZUID(args[2]), ZString(args[4]));
            } else {
                ELOG("Unknown type");
            }
        } else {
            ELOG("Failed to open");
        }

    } else if(args.size() > 0 && args[0] == "fetch"){
        if(args.size() != 3){
            LOG("Usage: fetch <file> <id>");
            return EXIT_FAILURE;
        }
//        LOG("Fetching Object from ZParcel " << args[1]);

        ZParcel parcel;
        bool ok = parcel.open(ZString(args[1]));
        if(ok){
//            LOG(args[2]);
            ZParcel::objtype type = parcel.getType(ZUID(args[2]));
            switch(type){
                case ZParcel::BOOLOBJ:
                    LOG(parcel.fetchBool(ZUID(args[2])));
                    break;
                case ZParcel::UINTOBJ:
                    LOG(parcel.fetchUint(ZUID(args[2])));
                    break;
                case ZParcel::SINTOBJ:
                    LOG(parcel.fetchSint(ZUID(args[2])));
                    break;
                case ZParcel::FLOATOBJ:
                    LOG(parcel.fetchFloat(ZUID(args[2])));
                    break;
                case ZParcel::ZUIDOBJ:
                    LOG(parcel.fetchZUID(ZUID(args[2])).str());
                    break;
                case ZParcel::STRINGOBJ:
                    LOG(parcel.fetchString(ZUID(args[2])));
                    break;
                case ZParcel::BLOBOBJ:
                    LOG(parcel.fetchBlob(ZUID(args[2])));
                    break;
                case ZParcel::FILEOBJ:
                    LOG(parcel.fetchFile(ZUID(args[2])));
                    break;
                default:
                    ELOG("Unknown type");
                    return EXIT_FAILURE;
                    break;
            }
        } else {
            ELOG("Failed to open");
            return EXIT_FAILURE;
        }

    } else if(args.size() > 0 && args[0] == "edit"){
        if(args.size() != 4){
            LOG("Usage: edit <file> <id> <value>");
            return EXIT_FAILURE;
        }
        LOG("Editing Object in ZParcel " << args[1]);

    } else {
        LOG("Commands:");
        LOG("    create <file>");
        LOG("    list <file>");
        LOG("    store <file> <id> <type> <value>");
        LOG("    fetch <file> <id>");
        LOG("    delete <file> <id>");
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv){
    try {
        return mainwrap(argc, argv);
    } catch(ZException err){
        ELOG(err.what());
        return EXIT_FAILURE;
    }
}
