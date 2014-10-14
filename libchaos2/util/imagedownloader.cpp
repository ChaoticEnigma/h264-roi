#include "zlog.h"
#include "zfile.h"
#include "zbinary.h"
using namespace LibChaos;

// Download multiple images from internet
// Args: <download prefix>

int main(int argc, char **argv){
    ZLog::formatStdout(ZLogSource::all, "%log%");

    // /////////////////////////

    ArZ param_args;
    AsArZ param_flags;

    for(int i = 0; i < argc; ++i){
        if(argv[i][0] == '-'){
            ZString tmp = argv[i];
            tmp.substr(1);
            ArZ fl = tmp.explode('=');
            ZString flg = fl[0];
            fl.popFront();
            ZString str = ZString::compound(fl, ZString());
            param_flags[flg] = str;
        } else {
            param_args.push(argv[i]);
        }
    }
    param_args.popFront();

    // /////////////////////////

    LOG("Downloading Images...");
    ZString url;
    url = "http://http://www.ietf.org/rfc/rfc4122.txt";
    ZString cmd = "wget --no-check-certificate -P ";
    cmd += param_args[0];
    cmd += " ";

    ZPath::makeDir(param_args[0]);

    ZString cd = "cd ";
    cd += param_args[0];
    LOG(cd);
    system(cd.cc());

    ZString cmdstr;
    int n = 1;
    do {
        cmdstr = cmd + url + ZString::ItoS(n, 10, 3) + ".jpg";
        LOG(cmdstr);
        ++n;
    } while(system(cmdstr.cc()) == 0);

    LOG("Finished");
    return 0;
}
