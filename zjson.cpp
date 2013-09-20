#include "zjson.h"

namespace LibChaos {

ZJSON::ZJSON(){}
ZJSON::ZJSON(ZString json){
    fromJSON(json);
}
ZJSON::ZJSON(AsArZ arr){
    dat() = arr.dat();
}

ZString ZJSON::toJSON(){
    ZString tmp;
    tmp << "{\"" << ZString(key(0)).replace("\"", "\\\"").str() << "\":\"" << at(0).replace("\"", "\\\"") << "\"";
    for(unsigned i = 1; i < size(); ++i){
        tmp << ",\"" << ZString(key(i)).replace("\"", "\\\"").str() << "\":\"" << at(i).replace("\"", "\\\"") << "\"";
    }
    tmp << "}";
    return tmp;
}

bool ZJSON::validJSON(ZString json){
    // JSON validation function and partner fromJSON()
    // All values must have keys: "":"", NOT "",
    // Every key / value must be a string literal with escaped "
    // You can have as many spaces or newlines as you want between keys and values or keys or opening or closing brackets
    ZString s = json;
    // loc is changed when specified character is hit under specific conditions
    enum Locat {
        start,
        firstc, // {
        skey, // "
        key, // any char but "
        ekey, // "
        colon, // :
        svalue, // "
        value, // any char but "
        evalue // "
        // JSON is valid if } is hit without errors earlier
    } loc = start;
    unsigned last = 0;
    //unsigned size = s.size();
    for(unsigned i = 0; i < s.size(); ++i){
        char c = s[i];
        switch(loc){
        case start:
            if(c != '{')
                return false;
            loc = firstc;
            break;
        case firstc:
            if(c == '"')
                loc = skey;
            else if(c != ' ' && c != '\n')
                return false;
            break;
        case skey:
            if(c == '"')
                return false;
            loc = key;
            break;
        case key:
            if(c == '"' && s[i-1] != '\\')
                loc = ekey;
            break;
        case ekey:
            if(c == ':')
                loc = colon;
            else if(c != ' ' && c != '\n')
                return false;
            break;
        case colon:
            if(c == '"')
                loc = svalue;
            else if(c != ' ' && c != '\n')
                return false;
            break;
        case svalue:
            if(c == '"' && s[i-1] != '\\')
                loc = evalue;
            else
                loc = value;
            break;
        case value:
            if(c == '"' && s[i-1] != '\\')
                loc = evalue;
            break;
        case evalue:
            if(c == ',')
                loc = firstc;
            else if(c == '}')
                return true;
            else if(c != ' ' && c != '\n')
                return false;
            break;
        default:
            // Not Good. Likely Memory Corruption.
            return false;
            break;
        }
        last = i;
    }
    return false;
}

ZJSON ZJSON::fromJSON(ZString json){
    if(!validJSON(json))
        return AsArZ();
    ZString s = json;
    enum Locat {
        firstc,
        key,
        ekey,
        value,
        evalue
    } loc = firstc;
    ZJSON final;
    ZString kbuff;
    ZString vbuff;
    for(unsigned i = 0; i < s.size(); ++i){
        char c = s[i];
        switch(loc){
        case firstc:
            if(c == '"')
                loc = key;
            break;
        case key:
            if(c == '"' && s[i-1] != '\\')
                loc = ekey;
            else
                kbuff << c;
            break;
        case ekey:
            if(c == '"')
                loc = value;
            break;
        case value:
            if(c == '"' && s[i-1] != '\\')
                loc = evalue;
            else
                vbuff << c;
            break;
        case evalue:
            {
                ZString kp = kbuff;
                kp.replace("\\\"", "\"");
                ZString vp = vbuff;
                vp.replace("\\\"", "\"");
                final.push(vp);
                final.key(final.size()-1) = kp.str();
                kbuff.clear();
                vbuff.clear();
                if(c == ','){
                    loc = firstc;
                } else if(c == '}'){
                    dat() = final.dat();
                    return *this;
                }
            }
            break;
        default:
            // Not Good.
            break;
        }
    }
    // This should not happen.
    return *this;
}

AsArZ ZJSON::toZAssoc(){
    AsArZ tmp;
    tmp.dat() = dat();
    return tmp;
}

}
