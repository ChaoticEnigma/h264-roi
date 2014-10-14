#include "../progress.h"
#include "../window.h"

namespace LibChaosUI {

ZProgress::ZProgress(int x_, int y_, int width_, int height_, zu16 range_) : handle(0), x(x_), y(y_), width(width_), height(height_),
    range(range_), progress(0), style(0), state(state_normal){}

void ZProgress::initialize(){
    mtx.lock();
    id = parent->getNewId();
    mtx.unlock();
}

bool ZProgress::create(){
    mtx.lock();
    handle = CreateWindowEx(0, PROGRESS_CLASS, NULL,
                            WS_CHILD | WS_VISIBLE | style,
                            x, y, width, height,
                            parent->getHandle(), (HMENU)(intptr_t)id, parent->getInstance(),
                            NULL);
    if(!handle){
        LOG("ZProgress window creation failed");
        return false;
    }
    SendMessage(handle, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
    SendMessage(handle, PBM_SETRANGE, 0, MAKELPARAM(0, range));
    SendMessage(handle, PBM_SETPOS, progress, 0);
    SendMessage(handle, PBM_SETSTATE, (zu32)state, 0);
    if(style & PBS_MARQUEE)
        SendMessage(handle, PBM_SETMARQUEE, (WPARAM)true, 0);
    mtx.unlock();
    return true;
}

bool ZProgress::destroy(){
    mtx.lock();
    DestroyWindow(handle);
    mtx.unlock();
    return true;
}

bool ZProgress::setProgress(zu16 prog){
    mtx.lock();
    if(prog > range)
        progress = range;
    else
        progress = prog;
    if(handle)
        SendMessage(handle, PBM_SETPOS, progress, 0);
    mtx.unlock();
    return true;
}

bool ZProgress::setMarquee(bool mode){
    mtx.lock();
    if(mode)
        style = PBS_MARQUEE;
    else
        style = 0;
    if(handle){
        update();
    }
    mtx.unlock();
    return true;
}

bool ZProgress::setState(bar_state st){
    mtx.lock();
    state = st;
    if(handle)
        SendMessage(handle, PBM_SETSTATE, (zu32)state, 0);
    mtx.unlock();
    return true;
}

} // namespace LibChaosUI
