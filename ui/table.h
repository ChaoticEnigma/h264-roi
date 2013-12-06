#ifndef ZUITABLE_H
#define ZUITABLE_H

#include "control.h"
#include "font.h"

namespace LibChaosUI {

class ZTable : public ZControl {
public:
    ZTable(int x, int y, int width, int height, int id = 0, ZFont font = ZFont());
    bool create();

    int addColumn(ZString header, int width);
private:
    void updateCols();

    HWND handle;
    int id;
    int x;
    int y;
    int width;
    int height;
    ZFont font;

    struct Col {
        ZString text;
        int width;
    };

    ZArray<Col> head;
    ZArray< ZArray<ZString> > cols;
};

}

#endif // ZUITABLE_H
