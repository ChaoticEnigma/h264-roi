/*******************************************************************************
**                                  LibChaos                                  **
**                                  zppm.cpp                                  **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#include "zppm.h"

namespace LibChaos {

bool ZPPM::read(ZPath path){

    return false;
}

bool ZPPM::write(ZPath path){
    FILE *file = fopen(path.str().cc(), "wb");
    if(file == NULL)
        return false;

    fprintf(file, "P6\n%llu %llu\n255\n", image.width(), image.height());

    for(zu64 y = 0; y < image.height(); ++y)
        fwrite(image.buffer() + (y * image.rowSize()), 1, image.rowSize(), file);

    fclose(file);
    return true;
}

}
