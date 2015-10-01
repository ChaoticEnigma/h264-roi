/*******************************************************************************
**                                  LibChaos                                  **
**                                  zppm.cpp                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zppm.h"
#include "zfile.h"
#include <stdio.h>

namespace LibChaos {

bool ZPPM::decode(ZBinary &ppmdata_in, YImageBackend::ReadOptions *options){

    return false;
}

bool ZPPM::encode(ZBinary &ppmdata_out, YImageBackend::WriteOptions *options){
    if(!_image->isRGB24()){
        throw ZException("Incorrect image pixel format");
        return false;
    }
    ZString header = "P6\n" + ZString::ItoS(_image->width()) + " " + ZString::ItoS(_image->height()) + "\n255\n";
    ppmdata_out.write(header.bytes(), header.size());
    for(zu64 y = 0; y < _image->height(); ++y)
        ppmdata_out.write(_image->buffer() + (y * _image->rowSize()), _image->rowSize());
    return true;
}

bool ZPPM::read(ZPath path){

    return false;
}

bool ZPPM::write(ZPath path){
    ZFile file(path, ZFile::WRITE);

    ZBinary out;
    encode(out);

    file.write(out);
    return true;
}

}
