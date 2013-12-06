#ifndef ZUIBUTTON_H
#define ZUIBUTTON_H

#include "control.h"

namespace LibChaosUI {

class ZButton : public ZControl {
public:
    ZButton(ZString text, int x, int y, int width, int height, int id = 0);

    void initialize();

    bool create();
    bool update();

    bool setText(ZString);

    typedef void (*zuiButtonClick)(ZButton *);
    void setClickHandler(zuiButtonClick);
    void clickCallback();
    void callback(int);
    int getId();
private:
    HWND handle;
    int id;
    ZString text;
    int x;
    int y;
    int width;
    int height;

    zuiButtonClick click;
};

}

#endif // ZUIBUTTON_H
