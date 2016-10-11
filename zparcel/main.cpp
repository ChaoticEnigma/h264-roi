/*******************************************************************************
**                                  LibChaos                                  **
**                              zparcel/main.cpp                              **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
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

//    LOG("ZParcel Command: \"" << argstr << "\"");

    if(args.size() > 0 && args[0] == "create"){
        if(args.size() != 2){
            ELOG("Usage: create <file>");
            return EXIT_FAILURE;
        }
//        LOG("Creating new ZParcel " << args[1]);

        ZPath file = ZString(args[1]);
        ZParcel parcel;
        ZParcel::parcelerror err = parcel.create(file);
        if(err == ZParcel::OK){
            LOG("OK - Create " << file);
        } else {
            LOG("FAIL - " << ZParcel::errorStr(err));
        }

    } else if(args.size() > 0 && args[0] == "list"){
        if(args.size() != 2){
            ELOG("Usage: list <file>");
            return EXIT_FAILURE;
        }
//        LOG("Listing Objects in ZParcel " << args[1]);

    } else if(args.size() > 0 && args[0] == "store"){
        if(args.size() != 5){
            ELOG("Usage: store <file> <id> <type> <value>");
            return EXIT_FAILURE;
        }
//        LOG("Storing Object in ZParcel " << args[1]);

        ZUID uid = args[2];
        ZString type = args[3];
        ZString value = args[4];

        if(uid == ZUID_NIL){
            ELOG("FAIL - Invalid UUID");
            return EXIT_FAILURE;
        }

        ZParcel parcel;
        ZParcel::parcelerror err;

        err = parcel.open(ZString(args[1]));
        if(err != ZParcel::OK){
            ELOG("FAIL - " << ZParcel::errorStr(err));
            return EXIT_FAILURE;
        }

        if(type == "null"){
            err = parcel.storeNull(uid);

        } else if(type == "bool"){
            err = parcel.storeBool(uid, (value == "true" ? true : false));

        } else if(type == "uint"){
            err = parcel.storeUint(uid, value.tozu64());

        } else if(type == "int" ||
                  type == "sint"){
            err = parcel.storeSint(uid, value.tint());

        } else if(type == "float" ||
                  type == "double"){
            err = parcel.storeFloat(uid, value.toFloat());

        } else if(type == "uid" ||
                  type == "uuid" ||
                  type == "zuid"){
            err = parcel.storeZUID(uid, value);

        } else if(type == "str" ||
                  type == "string"){
            err = parcel.storeString(uid, value);

        } else if(type == "bin" ||
                  type == "blob" ||
                  type == "binary"){
            ZBinary bin;
            ZFile::readBinary(value, bin);
            err = parcel.storeBlob(uid, bin);

        } else if(type == "file"){
            err = parcel.storeFile(uid, value);

        } else {
            ELOG("FAIL - Unknown type");
            return EXIT_FAILURE;
        }

        if(err != ZParcel::OK){
            ELOG("FAIL - Failed to store: " << ZParcel::errorStr(err));
            return EXIT_FAILURE;
        }

        LOG("OK " << uid.str());

    } else if(args.size() > 0 && args[0] == "fetch"){
        if(args.size() != 3){
            ELOG("Usage: fetch <file> <id>");
            return EXIT_FAILURE;
        }
//        LOG("Fetching Object from ZParcel " << args[1]);

        ZParcel parcel;
        ZParcel::parcelerror err = parcel.open(ZString(args[1]));
        if(err == ZParcel::OK){
//            LOG(args[2]);
            ZParcel::objtype type = parcel.getType(ZUID(args[2]));
            if(type == ZParcel::UNKNOWNOBJ){
                ELOG("FAIL - Bad object type");
                return EXIT_FAILURE;
            }
            switch(type){
                case ZParcel::NULLOBJ:
                    LOG("NULL");
                    break;
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
                case ZParcel::FILEOBJ: {
                    zu64 offset;
                    zu64 size;
                    ZString name = parcel.fetchFile(ZUID(args[2]), &offset, &size);
                    ZFile pfile = parcel.getHandle();
                    pfile.seek(offset);

                    ZFile file(ZFile::STDOUT);
                    ZBinary buff;
                    zu64 len = 0;
                    while(len < size){
                        buff.clear();
                        size = size - pfile.read(buff, MIN(1 << 15, size));
                        ZASSERT(file.write(buff), "write failed");
                    }
                    break;
                }
                default:
                    ELOG("Unknown type: " << ZParcel::typeName(type));
                    return EXIT_FAILURE;
                    break;
            }
        } else {
            ELOG("Failed to open: " << ZParcel::errorStr(err));
            return EXIT_FAILURE;
        }

    } else if(args.size() > 0 && args[0] == "edit"){
        if(args.size() != 4){
            ELOG("Usage: edit <file> <id> <value>");
            return EXIT_FAILURE;
        }
//        LOG("Editing Object in ZParcel " << args[1]);

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
