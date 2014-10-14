#include "../table.h"
#include "../window.h"

namespace LibChaosUI {

ZTable::ZTable(int x_, int y_, int width_, int height_, int id_, int gid_, ZFont _font) : head(0), list(0), id(id_), gid(gid_), x(x_), y(y_), width(width_), height(height_), font(_font){}

bool ZTable::create(){
    head = CreateWindowEx(0, WC_HEADER, NULL,
                            WS_CHILD | WS_VISIBLE | HDS_BUTTONS | HDS_HORZ,
                            x, y, width, 25,
                            parent->getHandle(), (HMENU)(intptr_t)id, parent->getInstance(),
                            NULL);
    SendMessage(head, WM_SETFONT, (WPARAM)font.handle(), MAKELPARAM(FALSE, 0));
    list = CreateWindowEx(0, WC_LISTVIEW, NULL,
                            WS_CHILD | LVS_REPORT | LVS_EDITLABELS,
                            x, y+25, width, height-25,
                            parent->getHandle(), (HMENU)(intptr_t)356, parent->getInstance(),
                            NULL);
    SendMessage(list, WM_SETFONT, (WPARAM)font.handle(), MAKELPARAM(FALSE, 0));
    updateCols();
    return true;
}

void ZTable::updateCols(){

    HDITEMW hditem;
    hditem.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
    hditem.fmt = HDF_CENTER | HDF_STRING;

    LVCOLUMNW lvc;
    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvc.fmt = LVCFMT_LEFT;

    for(zu64 i = 0; i < headcols.size(); ++i){
        hditem.cxy = headcols[i].width;
        hditem.pszText = (wchar_t *)headcols[i].text.wc();
        hditem.cchTextMax = sizeof(hditem.pszText) / sizeof(hditem.pszText[0]);
        SendMessage(head, HDM_INSERTITEM, (WPARAM)i, (LPARAM)&hditem);

        lvc.cx = headcols[i].width;
        lvc.pszText = (wchar_t *)headcols[i].text.wc();
        lvc.cchTextMax = sizeof(lvc.pszText) / sizeof(lvc.pszText[0]);
        lvc.iSubItem = i;
        LoadString(parent->getInstance(), gid + i, (wchar_t *)headcols[i].text.wc(), sizeof(hditem.pszText) / sizeof(hditem.pszText[0]));
        if(ListView_InsertColumn(list, i, &lvc)){
            LOG("BAD ListView_InsertColumn");
            return;
        }

        LVITEMW lvi;
        lvi.pszText   = LPSTR_TEXTCALLBACK;
        lvi.mask      = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
        lvi.stateMask = 0;
        lvi.iSubItem  = 0;
        lvi.state     = 0;

        for(zu64 j = 0; j < rows.size(); j++){
            lvi.iItem  = j;
            lvi.iImage = j;
            lvi.pszText = (wchar_t *)rows[j][i].wc();
            lvi.cchTextMax = sizeof(lvi.pszText) / sizeof(lvi.pszText[0]);

            if(ListView_InsertItem(list, &lvi)){
                LOG("BAD ListView_InsertItem");
                return;
            }

        }
    }
}

int ZTable::addColumn(ZString header, int width){
    Col col;
    col.text = header;
    col.width = width;
    headcols.push(col);
    if(head)
        updateCols();
    return headcols.size() - 1;
}
int ZTable::addRow(ZArray<ZString> cells){
    rows.push(cells);
    return rows.size()-1;
}

} // namespace LibChaosUI
