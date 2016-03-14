#include "test.h"
#include "zjson.h"

void checkType(ZJSON &json){
    switch(json.type()){
    case ZJSON::OBJECT: {
        LOG("Object: " << json.object().size());
        for(auto i = json.object().begin(); i.more(); i.advance()){
            LOG("O: " << i.get());
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

int json_test(){
    ZString str1 = "{ \"object\" : { \"str\" : \"strval\" , \"num\" : 12345 } , \"array\" : [ \"val1\" , \"val2\" ] , \"string\" : \"stringval\" , \"number\" : 54321 }";
    LOG(str1);
    ZJSON json1;
    bool ret = json1.decode(str1);
    LOG(ret);
    checkType(json1);

    ZJSON json2;
    json2.object()["string"] = ZJSON(ZJSON::STRING);
    json2.object()["string"].string() = "test1";
    json2.object()["number"] = ZJSON(ZJSON::NUMBER);
    json2.object()["number"].number() = "12345";
    json2.object()["object"] = ZJSON(ZJSON::OBJECT);
    json2.object()["object"].object()["str"] = ZJSON(ZJSON::STRING);
    json2.object()["object"].object()["str"].string() = "test2";
    json2.object()["array"] = ZJSON(ZJSON::ARRAY);
    json2.object()["array"].array().push(ZJSON(ZJSON::STRING));
    json2.object()["array"].array()[0].string() = "test3";
    json2.object()["array"].array().push(ZJSON(ZJSON::NUMBER));
    json2.object()["array"].array()[1].number() = "54321";
    LOG(json2.encode());

    return 0;
}
