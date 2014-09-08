#include "test.h"

#include "zpng.h"

int png_block(){

    ZPNG png;
    png.read_png_file("whois.png");
    //png.process_file();
    png.write_png_file("whois2.png");

    return 0;
}
