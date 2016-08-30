#include "main.h"
#include "zexception.h"

int mainwrap(int argc, char **argv){
    ZLog::logLevelStdOut(ZLogSource::NORMAL, "%log%");
    ZLog::logLevelStdOut(ZLogSource::DEBUG, "%time% (%file%:%line%) - %log%");
    ZLog::logLevelStdErr(ZLogSource::ERRORS, "%time% (%file%:%line%) - %log%");

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
        if(args.size() < 2 || args.size() > 3){
            LOG("Usage: create <file> [version]");
            return EXIT_FAILURE;
        }
        zu16 version = 4;
        if(args.size() > 2 && args[2] == "4")
            version = 4;

        if(version == 4){
            LOG("Creating New Version 4 ZParcel " << args[1]);
            if(ZFile::exists(args[1]))
                ZFile::remove(args[1]);
            ZFile file(args[1], ZFile::READWRITE);
            ZParcel4Parser *parcel = new ZParcel4Parser(&file);

            //parcel->setPageSize(11); // 2KB
            parcel->setPageSize(9); //512B
            parcel->setMaxPages(64 * 1024 * 2); // 128K pages

            parcel->create();
            LOG("OK");
            delete parcel;
        } else {
            LOG("Unknown Version");
        }

    } else if(args.size() > 0 && args[0] == "modify"){
        if(args.size() < 3){
            LOG("Usage: modify <file> <command>");
            return EXIT_FAILURE;
        }
        LOG("Modifying Options of ZParcel " << args[1]);
        ZFile file(args[1], ZFile::READWRITE);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);
        parcel->open();

        if(args[2] == "addfield"){
            if(args.size() != 5){
                LOG("Usage: modify <file> addfield <name> <type>");
                return EXIT_FAILURE;
            }
            ZString name = args[3];
            LOG("Adding New Field to ZParcel " << args[1]);
            ZParcel4Parser::fieldtype type = ZParcel::typeType(name);
            ZParcel4Parser::fieldid id = parcel->addField(name, type);
            if(id){
                LOG("Created field " << id << " - " << name << " : " << type);
                LOG("OK");
            } else {
                ELOG("Error creating field \"" << name << " : " << args[3]);
                LOG("ERROR");
            }
        } else {
            LOG("Usage: modify <file> <command>");
            LOG("Commands:");
            LOG("    addfield <name> <type>");
        }

        delete parcel;

    } else if(args.size() > 0 && args[0] == "list"){
        if(args.size() != 2){
            LOG("Usage: list <file>");
            return EXIT_FAILURE;
        }
        LOG("Listing Records in ZParcel " << args[1]);
        ZFile file(args[1], ZFile::READWRITE);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);
        parcel->open();

        LOG("Page Size: " << parcel->getPageSize());
        LOG("Max Pages: " << parcel->getMaxPages());

        delete parcel;

    } else if(args.size() > 0 && args[0] == "add"){
        if(args.size() < 2){
            LOG("Usage: add <file> [field[:type]=value] ...");
            return EXIT_FAILURE;
        }
        LOG("Adding Record to ZParcel " << args[1]);
        ZFile file(args[1], ZFile::READWRITE);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);
        parcel->open();

        ZList<ZParcel4Parser::Field> fieldlist;
        for(zu64 i = 2; i < args.size(); ++i){
            ArZ pair = args[i].split("=");
            if(pair.size() != 2)
                ELOG("Format error in \"" << args[i] << '"');
            ZString fieldname = pair[0];
            ZString fieldvalue = pair[1];
            ArZ fpair = fieldname.split(":");
            if(pair.size() == 2){
                fieldname = fpair[0];
                ZParcel::fieldtype type = ZParcel::typeType(fpair[1]);
                // Check if field exists, create if not, check that types are the same
            } else {
                // Check if field exists

            }
            //LOG(fieldname << "(" << ZParcel4Parser::getFieldTypeName(ftype) << ") : " << pair[1]);
            ZParcel4Parser::fieldid fid = parcel->getField(fieldname);
            if(fid){
                fieldlist.push({ fid, fieldvalue });
            } else {
                ELOG("Bad field \"" << fieldname << '"');
            }
        }

        for(zu64 i = 0; i < fieldlist.size(); ++i){
            LOG(fieldlist[i].id << " : " << fieldlist[i].data.size() << " bytes");
        }

        // Create record
        ZUID uid = parcel->addRecord(fieldlist);
        LOG("Added record " << uid.str());
        LOG("OK");

        delete parcel;

    } else if(args.size() > 0 && args[0] == "edit"){
        if(args.size() < 4){
            LOG("Usage: edit <file> <record> <field=value> [field=value] ..");
            return EXIT_FAILURE;
        }
        LOG("Editing Record in ZParcel " << args[1]);
        ZFile file(args[1], ZFile::READWRITE);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);
        parcel->open();

        delete parcel;

    } else {
        LOG("Commands:");
        LOG("    create <file>");
        LOG("    modify <file> <command>");
        LOG("    list <file>");
        LOG("    add <file> [field=value] ...");
        LOG("    edit <file> <record> [field=value] ...");
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
