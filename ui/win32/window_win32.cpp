#include "../window.h"
#include "../control.h"

namespace LibChaosUI {

HINSTANCE ZWindow::hinst = 0;

ZWindow::ZWindow(HINSTANCE hInstance, ShowMode mode) : windowclass(L"MainWindow"), hwnd(0), showmode(mode), id_seed(1), update(true){
    hinst = hInstance;
    INITCOMMONCONTROLSEX icc;
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icc);
}
ZWindow::~ZWindow(){
    for(zu64 i = 0; i < controls.size(); ++i){
        delete controls[i];
    }
}

bool ZWindow::createWindow(ZString title, int width, int height, zu16 icon_id, zu16 menu_id){
    windowtitle = title;

    WNDCLASSEX wc;
    wc.cbSize           = sizeof(WNDCLASSEX);
    wc.hInstance        = hinst;

    wc.style            = 0;
    wc.lpfnWndProc      = &ZWindow::windowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(this);
    wc.hInstance        = hinst;
    if(icon_id == 0)
        wc.hIcon        = NULL;
    else
        wc.hIcon        = (HICON)LoadImage(hinst, MAKEINTRESOURCE(icon_id), IMAGE_ICON, 32, 32, LR_SHARED);
    if(icon_id == 0)
        wc.hIconSm      = NULL;
    else
        wc.hIconSm      = (HICON)LoadImage(hinst, MAKEINTRESOURCE(icon_id), IMAGE_ICON, 16, 16, LR_SHARED);
    if(menu_id == 0)
        wc.lpszMenuName = MAKEINTRESOURCE(menu_id);
    else
        wc.lpszMenuName = MAKEINTRESOURCE(menu_id);

    //wc.hCursor          = (HCURSOR)LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground    = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hbrBackground    = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = windowclass;

    if(!RegisterClassEx(&wc)){
        LOG("Error registering window class: " << GetLastError());
        //MessageBox(NULL, L"Error registering window class.", L"Error", MB_ICONERROR | MB_OK);
        return false;
    }
    hwnd = CreateWindowEx(0, windowclass, windowtitle.wc(),
                          WS_OVERLAPPEDWINDOW,CW_USEDEFAULT, CW_USEDEFAULT,
                          width, height, NULL, NULL, hinst, this);
    if(!hwnd){
        LOG("Error creating main window: " << GetLastError());
        //MessageBox(NULL, L"Error creating main window.", L"Error", MB_ICONERROR | MB_OK);
        return false;
    }
    return true;
}
void ZWindow::show(){
    int cmdshow;
    switch(showmode.display){
        case ShowMode::show:
            if(activate)
                cmdshow = SW_SHOW;
            else
                cmdshow = SW_SHOWNA;
            break;
        case ShowMode::hide:
            cmdshow = SW_HIDE;
            break;
        case ShowMode::maximize:
            cmdshow = SW_MAXIMIZE;
            //cmdshow = SW_SHOWMAXIMIZED;
            break;
        case ShowMode::minimize:
            if(activate)
                cmdshow = SW_SHOWMINIMIZED;
            else
                cmdshow = SW_MINIMIZE;
            break;
        case ShowMode::restore:
            cmdshow = SW_RESTORE;
            break;
        case ShowMode::normal:
            if(activate)
                cmdshow = SW_SHOWNORMAL;
            else
                cmdshow = SW_SHOWNOACTIVATE;
            break;
        case ShowMode::def:
        default:
            cmdshow = SW_SHOWDEFAULT;
            break;
    }

    ShowWindow(hwnd, cmdshow);
    UpdateWindow(hwnd);
}

void ZWindow::add(ZControl *control){
    control->init(this);
    control->setChanged(true);
    controls.push(control);
    return;
}

void ZWindow::loadAccel(zu16 id){
    accel = LoadAccelerators(hinst, MAKEINTRESOURCE(id));
}

int ZWindow::exec(){
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) > 0){
        if(!TranslateAccelerator(msg.hwnd, accel, &msg)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

void ZWindow::setHandle(HWND hndl){
    hwnd = hndl;
}
HWND ZWindow::getHandle(){
    return hwnd;
}
HINSTANCE ZWindow::getInstance(){
    return hinst;
}
HACCEL ZWindow::getAccel(){
    return accel;
}
ZArray<ZControl *> &ZWindow::getControls(){
    return controls;
}

void ZWindow::setNeedUpdate(bool up){
    update = up;
}
bool ZWindow::needsUpdate(){
    return update;
}
void ZWindow::registerHandler(int param, ZControl *obj){
    if(obj != NULL)
        handlers[param] = obj;
    else
        handlers.erase(param);
}
std::map<int, ZControl *> *ZWindow::getHandlers(){
    return &handlers;
}
void ZWindow::setWindowData(void *ptr){
    windowdata = ptr;
}
void *ZWindow::getWindowData(){
    return windowdata;
}

ZWindow::ShowMode ZWindow::getWin32Mode(int nCmdShow){
    ShowMode mode;
    switch(nCmdShow){
        case SW_HIDE:
            mode.display = ShowMode::hide;
            mode.activate = false;
            break;
        case SW_MAXIMIZE:
        //case SW_SHOWMAXIMIZED:
            mode.display = ShowMode::maximize;
            mode.activate = true;
            break;
        case SW_MINIMIZE:
        //case SW_SHOWMINIMIZED:
            mode.display = ShowMode::minimize;
            mode.activate = true;
            break;
        case SW_SHOWMINNOACTIVE:
            mode.display = ShowMode::minimize;
            mode.activate = false;
            break;
        case SW_RESTORE:
            mode.display = ShowMode::restore;
            mode.activate = true;
            break;
        case SW_SHOW:
            mode.display = ShowMode::show;
            mode.activate = true;
            break;
        case SW_SHOWNA:
            mode.display = ShowMode::show;
            mode.activate = false;
            break;
        case SW_SHOWNORMAL:
            mode.display = ShowMode::normal;
            mode.activate = true;
            break;
        case SW_SHOWNOACTIVATE:
            mode.display = ShowMode::normal;
            mode.activate = false;
            break;
        default:
        case SW_SHOWDEFAULT:
            mode.display = ShowMode::def;
            mode.activate = true;
            break;
    }
    return mode;
}

}
