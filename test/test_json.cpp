#include "tests.h"
#include "zjson.h"

namespace LibChaosTest {

void checkType(ZJSON &json){
    switch(json.type()){
    case ZJSON::OBJECT: {
        LOG("Object: " << json.object().size());
        for(auto i = json.object().begin(); i.more(); i.advance()){
            LOG(i.get());
            checkType(json.object()[i.get()]);
        }
        break;
    }
    case ZJSON::ARRAY:
        LOG("Array");
        break;
    case ZJSON::STRING:
        LOG("String: " << json.string());
        break;
    case ZJSON::NUMBER:
        LOG("Number: " << json.number());
        break;
    case ZJSON::BOOLEAN:
        LOG("Boolean: " << json.boolean());
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

void json(){
    ZString str1 = "{ \"object\" : { \"str\" : \"strval\" , \"num\" : 12345 } , \"array\" : [ \"val1\" , \"val2\" ] , \"string\" : \"stringval\" , \"number\" : 54321 }";
    LOG(str1);
    ZJSON json1;
    bool ret = json1.decode(str1);
    LOG(ret);
    checkType(json1);

    ZJSON json2;
    json2["string"] = "test1";
    json2["number"] = 12345;
    json2["object"]["str"] = "test2";
    json2["array"] << "test\"3\"";
    json2["array"] << 54321;
    LOG(json2.encode());
}

ZArray<Test> json_tests(){
    return {
        { "json", json, true, {} },
    };
}

}
