/*******************************************************************************
**                                  LibChaos                                  **
**                                  zppm.cpp                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zppm.h"
#include "zfile.h"
#include <stdio.h>

#define PPM_SIG 0x5036

namespace LibChaos {

bool ZPPM::isPPM(const ZBinary &data){
    return (ZBinary::decle16(data.raw()) == PPM_SIG);
}

bool ZPPM::decode(ZReader *input){

    return false;
}

bool ZPPM::encode(ZWriter *output){
    if(!_image->isRGB24()){
        throw ZException("Incorrect image pixel format");
        return false;
    }

    ZString header = "P6\n" + ZString::ItoS(_image->width()) + " " + ZString::ItoS(_image->height()) + "\n255\n";
    output->write(header.bytes(), header.size());
    for(zu64 y = 0; y < _image->height(); ++y)
        output->write(_image->buffer() + (y * _image->rowSize()), _image->rowSize());
    return true;
}

}
