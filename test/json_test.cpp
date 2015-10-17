#include "test.h"
#include "zjson.h"

void checkType(ZJSON &json){
    switch(json.type()){
    case ZJSON::OBJECT:
        LOG("Object");
        break;
    case ZJSON::ARRAY:
        LOG("Array");
        break;
    case ZJSON::STRING:
        LOG("String");
        break;
    case ZJSON::NUMBER:
        LOG("Number");
        break;
    case ZJSON::BOOLEAN:
        LOG("Boolean");
        break;
    case ZJSON::NULLVAL:
        LOG("NULL");
        break;
    case ZJSON::UNDEF:
        LOG("Undefined");
        break;
    default:
        LOG("Other");
        break;
    }
}

int json_test(){
    ZString str1 = "{ \"object\" : { \"str\" : \"strval\" , \"num\" : 12345 } , \"string\" : \"stringval\" , \"number\" : 54321 }";
    ZJSON json1;
    json1.decode(str1);
    checkType(json1);
    return 0;
}
