#include "zsettings.h"

namespace LibChaos {

ZSettings::ZSettings(ZUID name_space) : _namespace(name_space){

}

ZSettings::ZSettings(ZPath file){
    // TODO
}

ZSettings::~ZSettings(){

}

bool ZSettings::hasOption(ZUID uid) const{
    return (_options.contains(uid) && _options[uid].set);
}

bool ZSettings::hasOption(ZString name) const {
    return (_names.contains(name) && hasOption(_names[name]));
}

bool ZSettings::hasOption(int key) const{
    return (_keys.contains(key) && hasOption(_keys[key]));
}

ZString ZSettings::getOption(ZUID uid, ZString fallback) const {
    if(hasOption(uid))
        return _options[uid].value;
    return fallback;
}

ZString ZSettings::getOption(ZString name, ZString fallback) const {
    if(_names.contains(name))
        return getOption(_names[name], fallback);
    return fallback;
}

ZString ZSettings::getOption(int key, ZString fallback) const {
    if(_keys.contains(key))
        return getOption(_keys[key], fallback);
    return fallback;
}

void ZSettings::setOption(ZUID uid, ZString value){
    Option &opt = _getOption(uid);
    opt.value = value;
    opt.set = true;
}

void ZSettings::setOption(ZString name, ZString value){
    setOption(_getUID(name), value);
}

void ZSettings::setOption(int key, ZString value){
    setOption(_getUID(key), value);
}

void ZSettings::unsetOption(ZUID uid){
    _getOption(uid).set = false;
}

void ZSettings::unsetOption(ZString name){
    unsetOption(_getUID(name));
}

void ZSettings::unsetOption(int key){
    unsetOption(_getUID(key));
}

ZUID ZSettings::registerOption(ZString name, int key, bool persist){
    ZUID uid = _makeUID(name);
    _names[name] = uid;
    _keys[key] = uid;
    _getOption(uid).persist = persist;
    return uid;
}

ZSettings::Option &ZSettings::_getOption(ZUID uid){
    if(!_options.contains(uid)){
        _options[uid].set = false;
        _options[uid].persist = false;
    }
    return _options[uid];
}

ZUID ZSettings::_getUID(ZString name){
    if(!_names.contains(name)){
        _names[name] = _makeUID(name);
    }
    return _names[name];
}

ZUID ZSettings::_getUID(int key){
    if(!_keys.contains(key)){
        _keys[key] = _makeUID(ZString::ItoS((zs64)key));
    }
    return _keys[key];
}

ZUID ZSettings::_makeUID(ZString name) const{
    return ZUID(ZUID::NAME, _namespace, name);
}

}
