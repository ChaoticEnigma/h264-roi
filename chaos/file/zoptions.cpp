#include "zoptions.h"

namespace LibChaos {

ZOptions::ZOptions(){

}

ZOptions::ZOptions(ZPath file){
    // TODO
}

ZOptions::~ZOptions(){

}

bool ZOptions::hasOption(ZUID uid){
    return _options.contains(uid);
}

bool ZOptions::hasOption(ZString name){
    return (_names.contains(name) && hasOption(_names[name]));
}

bool ZOptions::hasOption(int key){
    return (_keys.contains(key) && hasOption(_keys[key]));
}

ZString ZOptions::getOption(ZUID uid, ZString fallback){
    if(_options.contains(uid))
        return _options[uid];
    return fallback;
}

ZString ZOptions::getOption(ZString name, ZString fallback){
    if(_names.contains(name))
        return getOption(_names[name], fallback);
    return fallback;
}

ZString ZOptions::getOption(int key, ZString fallback){
    if(_keys.contains(key))
        return getOption(_keys[key], fallback);
    return fallback;
}

void ZOptions::setOption(ZUID uid, ZString value){
    _options[uid] = value;
}

void ZOptions::setOption(ZString name, ZString value){
    if(!_names.contains(name)){
        _names[name] = ZUID();
    }
    setOption(_names[name], value);
}

void ZOptions::setOption(int key, ZString value){

}

}
