#include "test.h"

#include "zpng.h"
#include "zbmp.h"

int png_block(){

    ZPNG png;
    LOG(png.libpngVersionInfo());
    //png.read_png_file("whois.png");
    //png.process_file();
    //png.write_png_file("whois2.png");
    png.read("whois.png");

    ZBMP bmp;
    bmp.read("tmp4-Z.bmp");
    bmp.write("out.bmp");

    return 0;
}
