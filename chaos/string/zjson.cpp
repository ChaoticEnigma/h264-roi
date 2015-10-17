/*******************************************************************************
**                                  LibChaos                                  **
**                                  zjson.cpp                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zjson.h"

#include <string>
#include <assert.h>

namespace LibChaos {

ZJSON::ZJSON() : type(UNDEF){

}

ZJSON::ZJSON(ZString str){
    decode(str);
}

ZJSON::ZJSON(const ZJSON &other) : type(other.type){
    switch(type){
    case OBJECT:
        data.object = other.data.object;
        break;
    case ARRAY:
        data.array = other.data.array;
        break;
    case STRING:
        data.string = other.data.string;
        break;
    case NUMBER:
        data.number = other.data.number;
        break;
    case BOOLEAN:
        data.boolean = other.data.boolean;
        break;
    }
}

ZJSON &ZJSON::operator=(ZString str){
    decode(str);
    return *this;
}

bool isWhitespace(char ch){
    return (ch == ' ' || ch == '\n' || ch == '\t');
}

bool isDigit(char ch){
    return (ch >= '0' && ch <= '9');
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
        akey, // "
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
                loc = akey;
            break;
        case akey:
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
    return (type != UNDEF);
}

bool ZJSON::decode(ZString s, zu64 *position){
//    if(!validJSON(s))
//        return false;

    zu64 pos = (position != nullptr ? *position : 0);

    // Check if JSON is true/false boolean or null
    ZString tstr = s.substr(pos);
    if(tstr.beginsWith("true", true)){
        type = BOOLEAN;
        data.boolean = true;
        return true;
    } else if(tstr.beginsWith("false", true)){
        type = BOOLEAN;
        data.boolean = false;
        return true;
    } else if(tstr.beginsWith("null", true)){
        type = NULLVAL;
        return true;
    }
    // Otherwise, parse

    // Location indicates what is expected at current character
    enum location {
        // Beginning of JSON string
        start,
        // Beginning of key
        bkey,
        // Inside key
        key,
        // At end of key
        akey,
        // Beginning of value
        bval,
        // Inside value
        //val,
        // At end of value
        aval,
        // Inside String Value
        str,
        // Inside Number Value
        num,
    } loc = start;

    // Key string buffer
    ZString kbuff;
    // Value buffer
    ZString vbuff;

    // Start decoding. If start is not null, start at given position
    for(zu64 i = pos; i < s.size(); ++i){
        char c = s[i];
        switch(loc){
        // Before JSON
        case start:
            if(c == '{'){
                type = OBJECT;
                loc = bkey;
            } else if(c == '['){
                type = ARRAY;
                loc = bval;
            } else if(c == '"'){
                type = STRING;
                loc = str;
            } else if(isDigit(c)){
                type = NUMBER;
                loc = num;
                vbuff += c;
            } else if(!isWhitespace(c)){
                return false;
            }
            break;

        // Before Key
        case bkey:
            if(c == '"'){
                loc = key;
            } else if(!isWhitespace(c)){
                return false;
            }
            break;

        // In Key
        case key:
            if(c == '"' && s[i-1] != '\\'){
                loc = akey;
            } else {
                kbuff += c;
            }
            break;

        // After Key
        case akey:
            if(c == ':'){
                loc = bval;
            } else if(!isWhitespace(c)){
                return false;
            }
            break;

        // Before Value
        case bval:
            ZJSON json;
            if(!json.decode(s, &i)){
                return false;
            }
            --i;
            if(type == OBJECT){
                data.object.add(kbuff, json);
                kbuff.clear();
            } else if(type == ARRAY){
                data.array.push(json);
            }
            type = aval;
            break;

        // String
        case str:
            if(c == '"' && s[i-1] != '\\'){
                data.string = vbuff;
                ++i;
                if(position != nullptr){
                    *position = i;
                }
                return true;
            } else {
                vbuff += c;
            }
            break;

        // Number
        case num:
            if(isWhitespace(c) || c == ',' || c == '}'){
                data.number = vbuff;
                if(position != nullptr){
                    *position = i;
                }
                return true;
            } else {
                vbuff += c;
            }
            break;

        // After Value
        case aval:
            if(c == ','){
                loc = bkey;
            } else if(c == '}'){
                if(position != nullptr){
                    *position = i;
                }
                return true;
            } else if(!isWhitespace(c)){
                return false;
            }
            break;

        default:
            // Not Good.
            assert(false);
            return false;
            break;
        }
    }
    return true;
}

ZString ZJSON::encode(){
    ZString tmp;

    return tmp;
}

ZJSON::JSONValue::JSONValue(){
    memset(this, 0, sizeof(JSONValue));
}

}
