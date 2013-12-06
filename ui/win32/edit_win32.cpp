#include "../edit.h"
#include "../window.h"

namespace LibChaosUI {

ZEdit::ZEdit(int x_, int y_, int width_, int height_, int id_, ZFont _font) : id(id_), x(x_), y(y_), width(width_), height(height_), font(_font){}

bool ZEdit::create(){
    handle = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,
                            x, y, width, height,
                            parent->getHandle(), (HMENU)(intptr_t)id, parent->getInstance(),
                            NULL);
    SendMessage(handle, WM_SETFONT, (WPARAM)font.handle(), MAKELPARAM(FALSE, 0));
    SendMessage(handle, WM_SETTEXT, 0, (LPARAM)L"Insert text here...");
    return true;
}
//bool ZUIEdit::update(){
//    return create();
//}

}
