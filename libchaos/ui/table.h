#ifndef ZUITABLE_H
#define ZUITABLE_H

#include "control.h"
#include "font.h"

namespace LibChaosUI {

class ZTable : public ZControl {
public:
    ZTable(int x, int y, int width, int height, int id = 0, int gid = 0, ZFont font = ZFont());
    bool create();

    int addColumn(ZString headcols, int width);
    int addRow(ZArray<ZString> cells);
private:
    void updateCols();

    HWND head;
    HWND list;
    int id;
    int gid;
    int x;
    int y;
    int width;
    int height;
    ZFont font;

    struct Col {
        ZString text;
        int width;
    };

    ZArray<Col> headcols;
    ZArray< ZArray<ZString> > rows;
};

} // namespace LibChaosUI

#endif // ZUITABLE_H
