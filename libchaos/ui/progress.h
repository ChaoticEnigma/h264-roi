#ifndef ZUIPROGRESS_H
#define ZUIPROGRESS_H

#include "control.h"

#define PBM_SETSTATE    (WM_USER+16)
#define PBST_NORMAL     0x0001
#define PBST_ERROR      0x0002
#define PBST_PAUSED     0x0003

namespace LibChaosUI {

class ZProgress : public ZControl {
public:
    ZProgress(int x, int y, int width, int height, zu16 range = 100);

    void initialize();
    bool create();
    bool destroy();

    bool setProgress(zu16 prog);
    bool setMarquee(bool);

    enum bar_state {
        state_normal =  PBST_NORMAL,
        state_error =   PBST_ERROR,
        state_paused =  PBST_PAUSED
    };

    bool setState(bar_state st);

    int getId(){
        mtx.lock();
        return id;
        mtx.unlock();
    }
private:
    HWND handle;
    int id;
    int x;
    int y;
    int width;
    int height;
    zu16 range;
    zu16 progress;
    zu32 style;
    bar_state state;
};

} // namespace LibChaosUI

#endif // ZUIPROGRESS_H
