#ifndef ZUIEDIT_H
#define ZUIEDIT_H

#include "control.h"
#include "font.h"

namespace LibChaosUI {

class ZEdit : public ZControl {
public:
    ZEdit(int x, int y, int width, int height, int id = 0, ZFont font = ZFont());
    bool create();
    //bool update();
private:
    HWND handle;
    int id;
    int x;
    int y;
    int width;
    int height;
    ZFont font;
};

}

#endif // ZUIEDIT_H
