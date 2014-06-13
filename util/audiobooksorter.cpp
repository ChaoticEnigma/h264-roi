#include "zlog.h"
#include "zfile.h"
#include "zbinary.h"
using namespace LibChaos;

zu64 getIntPrefix(ZString str){
    ZString val;
    for(zu64 i = 0; i < str.size(); ++i){
        char c = str[i];
        if( c == '0' ||
            c == '1' ||
            c == '2' ||
            c == '3' ||
            c == '4' ||
            c == '5' ||
            c == '6' ||
            c == '7' ||
            c == '8' ||
            c == '9'){
            val += c;
        } else {
            break;
        }
    }
    return (zu64)val.tint();
}

bool numericTest(ZString str1, ZString str2){
    if(getIntPrefix(str1) > getIntPrefix(str2)){
        return true;
    }
    return false;
}

bool pathNumericTest(ZPath path1, ZPath path2){
    for(zu64 i = 0; i < path1.size() && i < path2.size(); ++i){
        if(path1[i] == path2[i])
            continue;
        if(numericTest(path1[i], path2[i]))
            return true;
        return false;
    }
    if(path1.size() <= path2.size())
        return true;
    return false;
}

void sortNumeric(ArP &list){
    ArP tmppcfls;
    for(zu64 i = 0; i < list.size(); ++i){
        ZPath val = list[i];

        bool found = false;
        for(zu64 j = 0; j < tmppcfls.size(); ++j){
            ZPath val2 = tmppcfls[j];
            if(!pathNumericTest(val, val2)){
                tmppcfls.insert(j, list[i]);
                found = true;
                break;
            }
        }
        if(!found){
            tmppcfls.push(list[i]);
        }
    }
    list = tmppcfls;
}

int main(){
    ZLog::formatStdout(ZLogSource::all, "%log%");

    LOG("Fixing audiobook...");
    ZPath pwd  = ZPath::pwd();
    ZString name = pwd.last();
    LOG("Got name \"" << name << "\"");
    zu64 count = 0;
    ArP dirs = ZFile::listDirs(pwd, false);
    ArP files;
    for(zu64 i = 0; i < dirs.size(); ++i){
        files.concat(ZFile::listFiles(dirs[i], true));
    }

    LOG("Found " << files.size() << " files");

    sortNumeric(files);

    for(zu64 i = 0; i < files.size(); ++i){
        ZPath old = files[i];
        ZString newname = ZString::ItoS(i+1, 10, ZString::ItoS(files.size()+1).size()) + "_" + name + "_" + old.last();
        newname.replace(" ", "_");
        ZPath newpath = pwd + newname;

        ++count;

        LOG(old << " --> " << newpath);
    }

    LOG("Finished: moved " << count << " files.");
    return 0;
}

