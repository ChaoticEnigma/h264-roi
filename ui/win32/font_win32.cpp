#include "../font.h"

namespace LibChaosUI {

ZFont::ZFont(){
    font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
}

ZFont::ZFont(ZString name, int height, bool italic){
    font = CreateFont(height, 0, 0, 0, 0, italic, 0, 0, 0, 0, 0, 0, 0, name.wc());
}

ZFont::~ZFont(){
    //DeleteObject(font);
}

HFONT ZFont::handle(){
    return font;
}

}
