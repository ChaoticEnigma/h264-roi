#include "parcel-main.h"
#include "zexception.h"

using namespace ZParcelTypes;
//using namespace ZParcelConvert;

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

    } else if(args.size() > 0 && args[0] == "addfield"){
        if(args.size() != 4){
            LOG("Usage: addfield <file> <name> <type>");
            return EXIT_FAILURE;
        }
        LOG("Adding New Field to ZParcel");
        ZFile file(args[1], ZFile::modereadwrite);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);
        parcel->open();

        fieldtype type = ZParcel4Parser::fieldFileIdToFieldType(args[3].tint());
        fieldid id = parcel->addField(args[2], type);
        if(id){
            LOG("Created field " << id << " - " << args[2] << " : " << type);
            LOG("OK");
        } else {
            ELOG("Error creating field \"" << args[2] << ", " << args[3]);
            LOG("ERROR");
        }

        delete parcel;

    } else if(args.size() > 0 && args[0] == "add"){
        if(args.size() < 3){
            LOG("Usgae: add <file> <field=value> [field=value] ..");
            return EXIT_FAILURE;
        }
        LOG("Adding Record to ZParcel " << args[1]);
        ZFile file(args[1], ZFile::modereadwrite);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);
        parcel->open();

        FieldList fieldlist;
        for(zu64 i = 2; i < args.size(); ++i){
            ArZ pair = args[i].split("=");
            if(pair.size() != 2)
                ELOG("Format error in \"" << args[i] << '"');
            ZString fieldname = pair[0];
            ZString fieldvalue = pair[1];
            //LOG(fieldname << "(" << ZParcel4Parser::getFieldTypeName(ftype) << ") : " << pair[1]);
            fieldid fid = parcel->getFieldId(fieldname);
            if(fid){
                fieldlist.push({ fid, ZParcelConvert::toFileFormat(parcel->getFieldType(fid), fieldvalue) });
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

    } else if(args.size() > 0 && args[0] == "modify"){
        if(!(args.size() > 2)){
            LOG("Usgae: modify <file> <command>");
            return EXIT_FAILURE;
        }
        LOG("Modifying ZParcel Options" << args[1]);
        ZFile file(args[1], ZFile::modereadwrite);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);
        parcel->open();

        LOG(args[2]);

        delete parcel;

    } else if(args.size() > 0 && args[0] == "list"){
        if(!(args.size() > 1)){
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

    } else {
        LOG("Unknown Command");
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
