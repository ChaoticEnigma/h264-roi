#include "test.h"
#include "zjson.h"

int json_test(){
    ZString str = "{   \"text\" \n :  \"and text\"  ,   \"3453\"   :   \"456456\"   }";
    //str = "{  }";
    str = "{\"txt\":{\"more\":\"here\"}}";
    if(ZJSON::validJSON(str)){
        LOG("Valid JSON");
        ZJSON json(str);
        for(zu64 i = 0; i < json.size(); ++i){
            LOG("\"" << json.key(i) << "\" : \"" << json[i] << "\"");
        }
        LOG(json.encode());
    } else {
        LOG("Invalid JSON");
    }
    return 0;
}
