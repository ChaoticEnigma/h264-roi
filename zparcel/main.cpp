#include "main.h"
#include "zexception.h"

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

    LOG("ZParcel Command: \"" << cmdstr << "\"");

    if(args.size() > 0 && args[0] == "create"){
        if(args.size() != 2){
            LOG("Usgae: create <file>");
            return EXIT_FAILURE;
        }
        LOG("Creating New ZParcel " << args[1]);
        ZFile file(args[1], ZFile::modereadwrite);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);

        parcel->setPageSize(11);
        parcel->setMaxPages(64 * 1024 * 2);

        bool ok = parcel->create();
        if(ok)
            LOG("OK");
        else
            LOG("ERROR");

        delete parcel;

    } else if(args.size() > 0 && args[0] == "modify"){
        if(args.size() < 3){
            LOG("Usgae: modify <file> <command>");
            return EXIT_FAILURE;
        }
        LOG("Modifying Options of ZParcel " << args[1]);
        ZFile file(args[1], ZFile::modereadwrite);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);
        parcel->open();

        if(args[2] == "addfield"){
            if(args.size() != 5){
                LOG("Usage: modify <file> addfield <name> <type>");
                return EXIT_FAILURE;
            }
            ZString name = args[3];
            LOG("Adding New Field to ZParcel " << args[1]);
            ZMap<ZString, ZParcel4Parser::fieldtype> ftype = {
                { "bool",   ZPARCEL_4_BOOL },
                { "uint",   ZPARCEL_4_UINT },
                { "sint",   ZPARCEL_4_SINT },
                { "float",  ZPARCEL_4_FLOAT },
                { "zuid",   ZPARCEL_4_ZUID },
                { "string", ZPARCEL_4_STRING },
                { "blob",   ZPARCEL_4_BINARY },
                { "file",   ZPARCEL_4_FILE },
            };
            ZParcel4Parser::fieldtype type = ftype[name];
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
            LOG("Usgae: list <file>");
            return EXIT_FAILURE;
        }
        LOG("Listing Records in ZParcel " << args[1]);
        ZFile file(args[1], ZFile::modereadwrite);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);
        parcel->open();

        LOG("Page Size: " << parcel->getPageSize());
        LOG("Max Pages: " << parcel->getMaxPages());

        delete parcel;

    } else if(args.size() > 0 && args[0] == "add"){
        if(args.size() < 2){
            LOG("Usgae: add <file> [field=value] ...");
            return EXIT_FAILURE;
        }
        LOG("Adding Record to ZParcel " << args[1]);
        ZFile file(args[1], ZFile::modereadwrite);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);
        parcel->open();

        ZParcel4Parser::FieldList fieldlist;
        for(zu64 i = 2; i < args.size(); ++i){
            ArZ pair = args[i].split("=");
            if(pair.size() != 2)
                ELOG("Format error in \"" << args[i] << '"');
            ZString fieldname = pair[0];
            ZString fieldvalue = pair[1];
            //LOG(fieldname << "(" << ZParcel4Parser::getFieldTypeName(ftype) << ") : " << pair[1]);
            ZParcel4Parser::fieldid fid = parcel->getFieldId(fieldname);
            if(fid){
                ZParcel4Parser::fieldtype type = parcel->getFieldType(fid);
                switch(type){
                    case ZPARCEL_4_BOOL:
                        parcel->addBoolRecord(fid, (fieldvalue == "true" ? true : false));
                        break;
                    case ZPARCEL_4_UINT:
                        parcel->addUintRecord(fid, fieldvalue.tozu64());
                        break;
                    case ZPARCEL_4_SINT:
                        parcel->addSintRecord(fid, (zs64)fieldvalue.tozu64());
                        break;
                    case ZPARCEL_4_FLOAT:
                        parcel->addFloatRecord(fid, fieldvalue.toFloat());
                        break;
                    case ZPARCEL_4_ZUID:
                        parcel->addZUIDRecord(fid, ZUID(fieldvalue));
                        break;
                    case ZPARCEL_4_STRING:
                        parcel->addStringRecord(fid, fieldvalue);
                        break;
                    case ZPARCEL_4_BINARY:

                        break;
                    case ZPARCEL_4_FILE:
                        parcel->addFileRecord(fid, ZPath(fieldvalue));
                        break;
                    default:
                        ELOG("Unsupported type");
                }
            } else {
                ELOG("Bad field \"" << fieldname << '"');
            }
        }

        for(zu64 i = 0; i < fieldlist.size(); ++i){
            LOG(fieldlist[i].id << " : " << fieldlist[i].data.size() << " bytes");
        }

        bool ok = parcel->addRecord(fieldlist);
        if(ok)
            LOG("OK");
        else
            ELOG("ERROR");

        delete parcel;

    } else if(args.size() > 0 && args[0] == "edit"){
        if(args.size() < 4){
            LOG("Usgae: edit <file> <record> <field=value> [field=value] ..");
            return EXIT_FAILURE;
        }
        LOG("Editing Record in ZParcel " << args[1]);
        ZFile file(args[1], ZFile::modereadwrite);
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
