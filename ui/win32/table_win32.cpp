#include "../table.h"
#include "../window.h"

namespace LibChaosUI {

ZTable::ZTable(int x_, int y_, int width_, int height_, int id_, ZFont _font) : id(id_), x(x_), y(y_), width(width_), height(height_), font(_font){}

bool ZTable::create(){
    handle = CreateWindowEx(0, WC_HEADER, NULL,
                            WS_CHILD | WS_VISIBLE | WS_BORDER | HDS_BUTTONS | HDS_HORZ,
                            x, y, width, 25,
                            parent->getHandle(), (HMENU)(intptr_t)id, parent->getInstance(),
                            NULL);
    SendMessage(handle, WM_SETFONT, (WPARAM)font.handle(), MAKELPARAM(FALSE, 0));
    updateCols();
    return true;
}

void ZTable::updateCols(){
    for(zu64 i = 0; i < head.size(); ++i){
        HDITEMW hditem;
        hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
        hditem.cxy = head[i].width;
        hditem.pszText = (wchar_t *)head[i].text.wc();
        hditem.cchTextMax = sizeof(hditem.pszText) / sizeof(hditem.pszText[0]);
        hditem.fmt = HDF_CENTER | HDF_STRING;
        SendMessage(handle, HDM_INSERTITEM, (WPARAM)i, (LPARAM)&hditem);
    }
}

int ZTable::addColumn(ZString header, int width){
    Col col;
    col.text = header;
    col.width = width;
    head.push(col);
    if(handle)
        updateCols();
    return head.size() - 1;
}

}
