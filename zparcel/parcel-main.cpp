#include "parcel-main.h"
#include "zexception.h"

using namespace ZParcelTypes;
using namespace ZParcelConvert;

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
        if(!(args.size() > 1)){
            LOG("Usgae: create <file>");
            return EXIT_FAILURE;
        }
        LOG("Creating New ZParcel " << args[1]);
        ZFile file(args[1], ZFile::modereadwrite);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);

        parcel->setPageSize(11);
        parcel->setMaxPages(64 * 1024 * 2);
        parcel->create();

        delete parcel;
    } else if(args.size() > 0 && args[0] == "add"){
        if(!(args.size() > 2)){
            LOG("Usgae: add <file> <field:value> [field:value] ..");
            return EXIT_FAILURE;
        }
        LOG("Adding Record to ZParcel " << args[1]);
        ZFile file(args[1], ZFile::modereadwrite);
        ZParcel4Parser *parcel = new ZParcel4Parser(&file);

        parcel->open();
        FieldList fieldlist;
        for(zu64 i = 2; i < args.size(); ++i){
            ArZ pair = args[i].split("=");
            ArZ fieldpair = pair[0].split(":");
            ZString fieldname = fieldpair[0];
            zu16 newfieldtype = (fieldpair.size() == 2 ? (zu16)fieldpair[1].tozu64() : 0);
            fieldtype ftype = ZParcel4Parser::fieldFileIdToFieldType(newfieldtype);
            //LOG(fieldname << "(" << ZParcel4Parser::getFieldTypeName(ftype) << ") : " << pair[1]);
            fieldid fid = parcel->getFieldId(fieldname);
            if(fid == 0){
                if(ftype == nullfield){
                    ELOG("Invalid field type");
                } else {
                    LOG("New field: " << fieldname << "(" << ZParcel4Parser::getFieldTypeName(ftype) << ")");
                    fid = parcel->addField(fieldname, ftype);
                }
            } else {
                if(ftype != parcel->getFieldType(fid))
                    ELOG("Different field type specifid: " << ZParcel4Parser::getFieldTypeName(ftype) << ", " << ZParcel4Parser::getFieldTypeName(parcel->getFieldType(fid)));
            }
            if(fid != 0 && ftype != nullfield)
                fieldlist.push({ fid, toFileFormat(ftype, pair[1]) });
        }

        for(zu64 i = 0; i < fieldlist.size(); ++i){
            LOG(fieldlist[i].id << " : " << fieldlist[i].data.size() << " bytes");
        }

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
