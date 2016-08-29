#include "main.h"
#include "zexception.h"
#include "zlog.h"

using namespace LibChaos;

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
        if(args.size() < 2 || args.size() > 3){
            LOG("Usage: create <file> [version]");
            return EXIT_FAILURE;
        }

    } else if(args.size() > 0 && args[0] == "modify"){
        if(args.size() < 3){
            LOG("Usage: modify <file> <command>");
            return EXIT_FAILURE;
        }
        LOG("Modifying Options of ZParcel " << args[1]);

    } else if(args.size() > 0 && args[0] == "list"){
        if(args.size() != 2){
            LOG("Usage: list <file>");
            return EXIT_FAILURE;
        }
        LOG("Listing Records in ZParcel " << args[1]);

    } else if(args.size() > 0 && args[0] == "add"){
        if(args.size() < 2){
            LOG("Usage: add <file> [field[:type]=value] ...");
            return EXIT_FAILURE;
        }
        LOG("Adding Record to ZParcel " << args[1]);

    } else if(args.size() > 0 && args[0] == "edit"){
        if(args.size() < 4){
            LOG("Usage: edit <file> <record> <field=value> [field=value] ..");
            return EXIT_FAILURE;
        }

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
