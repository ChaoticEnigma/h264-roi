/*******************************************************************************
**                                  LibChaos                                  **
**                                  zjpeg.cpp                                 **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#include "zjpeg.h"
#include "zfile.h"

namespace LibChaos {

bool ZJPEG::decode(ZBinary &jpegdata_in){

    return true;
}

bool ZJPEG::encode(ZBinary &jepgdata_out, JPEGWrite::jpegoptions options){

    return true;
}

bool ZJPEG::read(ZPath path){
    ZBinary data;
    ZFile::readBinary(path, data);
    if(!data.size())
        error = ZException("JPEG Read: empty read file", JPEGError::badreadfile, false);

    return decode(data);
}

bool ZJPEG::write(ZPath path, JPEGWrite::jpegoptions options){
    ZBinary data;
    if(!encode(data, options))
        return false;

    if(!ZFile::writeBinary(path, data))
        error = ZException("JPEG Read: cannot write file", JPEGError::badwritefile, false);

    return true;
}

}
