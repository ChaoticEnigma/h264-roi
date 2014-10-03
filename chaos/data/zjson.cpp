#include "zjson.h"

#include <cassert>

namespace LibChaos {

ZJSON::ZJSON(){}
ZJSON::ZJSON(ZString str){
    decode(str);
}
ZJSON::ZJSON(AsArZ arr){
    data() = arr.data();
}

ZJSON &ZJSON::operator=(ZString str){
    return decode(str);
}

bool isWhitespace(char wsp){
    if(wsp == ' ' || wsp == '\n' || wsp == '\t'){
        return true;
    }
    return false;
}

bool ZJSON::validJSON(ZString s){
    // JSON validation function and partner fromJSON()
    // All values must have keys: "":"", NOT "",
    // Every key / value must be a string literal with escaped "
    // You can have as many spaces or newlines as you want between keys and values or keys or opening or closing brackets

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
    //unsigned last = 0;
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
            else if(c == '}')
                return true;
            else if(!isWhitespace(c))
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
            else if(!isWhitespace(c))
                return false;
            break;
        case colon:
            if(c == '"')
                loc = svalue;
            else if(!isWhitespace(c))
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
            else if(!isWhitespace(c))
                return false;
            break;
        default:
            // Not Good. Likely Memory Corruption.
            return false;
            break;
        }
        //last = i;
    }
    return false;
}

bool ZJSON::isValid(){
    return validJSON(json);
}

ZJSON ZJSON::fromJSON(ZString s){
    if(!validJSON(s))
        return ZJSON();
    enum Locat {
        firstc,
        key_,
        ekey,
        value,
        evalue
    } loc = firstc;
    ZJSON final;
    ZString kbuff;
    ZString vbuff;
    for(zu64 i = 0; i < s.size(); ++i){
        char c = s[i];
        switch(loc){
        case firstc:
            if(c == '"')
                loc = key_;
            break;
        case key_:
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
            if(c == ',' || c == '}'){
                final.push(ZString(vbuff).replace("\\\"", "\""));
                final.key(final.size()-1) = ZString(kbuff).replace("\\\"", "\"").str();
                kbuff.clear();
                vbuff.clear();
                if(c == ','){
                    loc = firstc;
                } /*else if(c == '}'){
                    dat() = final.dat();
                }*/
            }
            break;
        default:
            // Not Good.
            assert(false);
            break;
        }
    }
    return final;
}

ZJSON &ZJSON::decode(ZString str){
    json = str;
    data() = fromJSON(str).data();
    return *this;
}

ZString ZJSON::encode(){
    ZString tmp;
    tmp << "{";
    //tmp << "\"" << ZString(key(0)).replace("\"", "\\\"").str() << "\":\"" << at(0).replace("\"", "\\\"") << "\"";
    for(unsigned i = 0; i < size(); ++i){
        if(i != 0)
            tmp << ",";
        tmp << "\"" << ZString(key(i)).replace("\"", "\\\"").str() << "\":\"" << at(i).replace("\"", "\\\"") << "\"";
    }
    tmp << "}";
    return tmp;
}

AsArZ ZJSON::toZAssoc(){
    AsArZ tmp;
    tmp.data() = data();
    return tmp;
}

}
