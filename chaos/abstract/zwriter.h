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

    // Single Byte
    void writeu8(zu8 num){
        write(&num, 1);
    }
    void writes8(zu8 num){
        write(&num, 1);
    }

    // Little-endian Unsigned
    void writeleu16(zu16 num){
        zbyte tmp[2];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        write(tmp, 2);
    }
    void writeleu32(zu32 num){
        zbyte tmp[4];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        tmp[2] = (num >> 16) & 0xFF;
        tmp[3] = (num >> 24) & 0xFF;
        write(tmp, 4);
    }
    void writeleu64(zu64 num){
        zbyte tmp[8];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        tmp[2] = (num >> 16) & 0xFF;
        tmp[3] = (num >> 24) & 0xFF;
        tmp[4] = (num >> 32) & 0xFF;
        tmp[5] = (num >> 40) & 0xFF;
        tmp[6] = (num >> 48) & 0xFF;
        tmp[7] = (num >> 56) & 0xFF;
        write(tmp, 8);
    }

    // Little-endian Signed
    void writeles16(zu16 num){
        zbyte tmp[2];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        write(tmp, 2);
    }
    void writeles32(zu32 num){
        zbyte tmp[4];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        tmp[2] = (num >> 16) & 0xFF;
        tmp[3] = (num >> 24) & 0xFF;
        write(tmp, 4);
    }
    void writeles64(zu64 num){
        zbyte tmp[8];
        tmp[0] = num & 0xFF;
        tmp[1] = (num >> 8) & 0xFF;
        tmp[2] = (num >> 16) & 0xFF;
        tmp[3] = (num >> 24) & 0xFF;
        tmp[4] = (num >> 32) & 0xFF;
        tmp[5] = (num >> 40) & 0xFF;
        tmp[6] = (num >> 48) & 0xFF;
        tmp[7] = (num >> 56) & 0xFF;
        write(tmp, 8);
    }

    // Big-endian Unsigned
    void writebeu16(zu16 num){
        zbyte tmp[2];
        tmp[0] = (num >> 8) & 0xFF;
        tmp[1] = num & 0xFF;
        write(tmp, 2);
    }
    void writebeu32(zu32 num){
        zbyte tmp[4];
        tmp[0] = (num >> 24) & 0xFF;
        tmp[1] = (num >> 16) & 0xFF;
        tmp[2] = (num >> 8) & 0xFF;
        tmp[3] = num & 0xFF;
        write(tmp, 4);
    }
    void writebeu64(zu64 num){
        zbyte tmp[8];
        tmp[0] = (num >> 56) & 0xFF;
        tmp[1] = (num >> 48) & 0xFF;
        tmp[2] = (num >> 40) & 0xFF;
        tmp[3] = (num >> 32) & 0xFF;
        tmp[4] = (num >> 24) & 0xFF;
        tmp[5] = (num >> 16) & 0xFF;
        tmp[6] = (num >> 8) & 0xFF;
        tmp[7] = num & 0xFF;
        write(tmp, 8);
    }

    // Big-endian Signed
    void writebes16(zu16 num){
        zbyte tmp[2];
        tmp[0] = (num >> 8) & 0xFF;
        tmp[1] = num & 0xFF;
        write(tmp, 2);
    }
    void writebes32(zu32 num){
        zbyte tmp[4];
        tmp[0] = (num >> 24) & 0xFF;
        tmp[1] = (num >> 16) & 0xFF;
        tmp[2] = (num >> 8) & 0xFF;
        tmp[3] = num & 0xFF;
        write(tmp, 4);
    }
    void writebes64(zu64 num){
        zbyte tmp[8];
        tmp[0] = (num >> 56) & 0xFF;
        tmp[1] = (num >> 48) & 0xFF;
        tmp[2] = (num >> 40) & 0xFF;
        tmp[3] = (num >> 32) & 0xFF;
        tmp[4] = (num >> 24) & 0xFF;
        tmp[5] = (num >> 16) & 0xFF;
        tmp[6] = (num >> 8) & 0xFF;
        tmp[7] = num & 0xFF;
        write(tmp, 8);
    }

};

}

#endif // ZWRITER_H
