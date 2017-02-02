#include "zoptions.h"

namespace LibChaos {

ZOptions::ZOptions(ZUID name_space) : _namespace(name_space){

}

ZOptions::ZOptions(ZPath file){
    // TODO
}

ZOptions::~ZOptions(){

}

bool ZOptions::hasOption(ZUID uid) const{
    return (_options.contains(uid) && _options[uid].set);
}

bool ZOptions::hasOption(ZString name) const {
    return (_names.contains(name) && hasOption(_names[name]));
}

bool ZOptions::hasOption(int key) const{
    return (_keys.contains(key) && hasOption(_keys[key]));
}

ZString ZOptions::getOption(ZUID uid, ZString fallback) const {
    if(hasOption(uid))
        return _options[uid].value;
    return fallback;
}

ZString ZOptions::getOption(ZString name, ZString fallback) const {
    if(_names.contains(name))
        return getOption(_names[name], fallback);
    return fallback;
}

ZString ZOptions::getOption(int key, ZString fallback) const {
    if(_keys.contains(key))
        return getOption(_keys[key], fallback);
    return fallback;
}

void ZOptions::setOption(ZUID uid, ZString value){
    Option &opt = _getOption(uid);
    opt.value = value;
    opt.set = true;
}

void ZOptions::setOption(ZString name, ZString value){
    setOption(_getUID(name), value);
}

void ZOptions::setOption(int key, ZString value){
    setOption(_getUID(key), value);
}

void ZOptions::unsetOption(ZUID uid){
    _getOption(uid).set = false;
}

void ZOptions::unsetOption(ZString name){
    unsetOption(_getUID(name));
}

void ZOptions::unsetOption(int key){
    unsetOption(_getUID(key));
}

ZUID ZOptions::registerOption(ZString name, int key, bool persist){
    ZUID uid = _makeUID(name);
    _names[name] = uid;
    _keys[key] = uid;
    _getOption(uid).persist = persist;
    return uid;
}

ZOptions::Option &ZOptions::_getOption(ZUID uid){
    if(!_options.contains(uid)){
        _options[uid].set = false;
        _options[uid].persist = false;
    }
    return _options[uid];
}

ZUID ZOptions::_getUID(ZString name){
    if(!_names.contains(name)){
        _names[name] = _makeUID(name);
    }
    return _names[name];
}

ZUID ZOptions::_getUID(int key){
    if(!_keys.contains(key)){
        _keys[key] = _makeUID(ZString::ItoS((zs64)key));
    }
    return _keys[key];
}

ZUID ZOptions::_makeUID(ZString name) const{
    return ZUID(ZUID::NAME, _namespace, name);
}

}
