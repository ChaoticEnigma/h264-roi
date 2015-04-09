/*******************************************************************************
**                                  LibChaos                                  **
**                                  zwriter.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef ZWRITER_H
#define ZWRITER_H

#include "ztypes.h"

#include "zreader.h"
#include "zallocator.h"

namespace LibChaos {

class ZWriter {
public:
    virtual ~ZWriter(){}

    // Write <size> bytes from <src> to object at current position
    virtual zu64 write(const zbyte *src, zu64 size) = 0;

    // Read <size> bytes from <reader> and write to <this> in blocks of <blocksize>
    // Returns number of bytes written to <this>
    // Should be used to read/write between devices whose storage is out of memory
    virtual zu64 readWrite(ZReader *reader, zu64 size, zu64 blocksize = (1 << 15)){
        zbyte *buffer = new zbyte[blocksize];
        zu64 totalwritten = 0;
        while(totalwritten < size){
            // Read into buffer
            zu64 readsize = MAX(blocksize, size - totalwritten);
            if(reader->read(buffer, readsize) != readsize){
                break;
            }
            // Write
            zu64 written = write(buffer, readsize);
            totalwritten += written;
            if(written != readsize){
                break;
            }
        }
        delete[] buffer;
        return totalwritten;
    }
};

}

#endif // ZWRITER_H
