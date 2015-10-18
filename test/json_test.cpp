#include "test.h"
#include "zjson.h"

void checkType(ZJSON &json){
    switch(json.type()){
    case ZJSON::OBJECT:
        LOG("Object: " << json.object().size());
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
    ZString str1 = "{ \"object\" : { \"str\" : \"strval\" , \"num\" : 12345 } , \"array\" : [ \"val1\" , \"val2\" ] , \"string\" : \"stringval\" , \"number\" : 54321 }";
    LOG(str1);
    ZJSON json1;
    bool ret = json1.decode(str1);
    LOG(ret);
    checkType(json1);
    return 0;
}
