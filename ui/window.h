#ifndef ZUIWINDOW_H
#define ZUIWINDOW_H

#include "base.h"

#include <map>

namespace LibChaosUI {

class ZControl;

class ZWindow {
public:
    typedef zs64 (*messageProcType)(ZWindow *, zuint, zu64, zs64);
    typedef std::map<int, ZControl *> ControlMap;

    struct ShowMode {
        enum Show {
            def = 0,        // Default
            show = 1,       // Display with curent size / pos
            hide = 2,       // Hide (never activate)
            maximize = 3,   // Maximize (always activate)
            minimize = 4,   // Minimize
            restore = 5,    // Restore to original size / pos (always activate)
            normal = 6      // Display with original size / pos
        } display;
        bool activate;
    };

    ZWindow(HINSTANCE hinst, ShowMode mode = { ShowMode::def, true });
    ~ZWindow();
    bool createWindow(ZString title, int width = 800, int height = 450, zu16 icon_id = 0, zu16 menu_id = 0);
    void show();
    void add(ZControl *control);
    void loadAccel(zu16);
    int exec();

    int getNewId();

#if PLATFORM == WINDOWS
    // internal stuffs
    void setHandle(HWND hndl);
    HWND getHandle();
    static HINSTANCE getInstance();
    HACCEL getAccel();
    ZArray<ZControl *> getControls();
    void setNeedUpdate(bool up);
    bool needsUpdate();
    void registerHandler(int, ZControl *);
    ControlMap *getHandlers();
    void setWindowData(void *);
    void *getWindowData();

    static ShowMode getWin32Mode(int nCmdShow);
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

private:
    LPCWSTR windowclass;
    HWND hwnd;
    static HINSTANCE hinst;
    ztid thread;
    ShowMode showmode;
    bool activate;
    ZString windowtitle;
    HACCEL accel;
    int id_seed;
    bool update;
    ZArray<ZControl *> controls;
    ControlMap handlers;
    void *windowdata;
};

} // namespace LibChaosUI

#endif // ZUIWINDOW_H
