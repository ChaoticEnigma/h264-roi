#include "zpdf.h"
#include "zlog.h"

namespace LibChaos {

ZPDF::ZPDF(ZPath file){
    _file.open(file);
    read();
}

ZPDF::~ZPDF(){
    _file.close();
}

void ZPDF::list() const {
    LOG("Lines: " << _lines.size());
}

void ZPDF::read(){
    ZBinary data;
    ZFile::readBinary(_file.path(), data);
    ZBinary buffer;
    for(zu64 i = 0; i < data.size(); ++i){
        if(data[i] == '\n'){
            _lines.push(buffer);
            buffer.clear();
        } else {
            buffer.append(data[i]);
        }
    }
    _lines.push(buffer);
}

}
