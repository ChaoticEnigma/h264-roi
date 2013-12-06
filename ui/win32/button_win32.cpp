#include "../button.h"
#include "../window.h"

namespace LibChaosUI {

ZButton::ZButton(ZString text_, int x_, int y_, int width_, int height_, int id_) : handle(0), id(id_), text(text_), x(x_), y(y_), width(width_), height(height_), click(NULL){}

void ZButton::initialize(){
    LOG("Button init");
    if(click != NULL){
        parent->registerHandler(id, this);
    }
}

bool ZButton::create(){
    handle = CreateWindowEx(0, L"BUTTON", text.wc(),
                            WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                            x, y, width, height,
                            parent->getHandle(), (HMENU)(intptr_t)id, parent->getInstance(),
                            NULL);
    if(!handle)
        return false;
    SendMessage(handle, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
    return true;
}
bool ZButton::update(){
    DestroyWindow(handle);
    return create();
}

bool ZButton::setText(ZString txt){
    text = txt;
    //SetWindowText(handle, text.wc());
    setChanged(true);
    return true;
}

void ZButton::setClickHandler(zuiButtonClick func){
    click = func;
    if(parent != NULL){
        if(click != NULL)
            parent->registerHandler(id, this);
        else
            parent->registerHandler(id, NULL);
    }
}

void ZButton::clickCallback(){
    if(click != NULL){
        click(this);
    }
}
void ZButton::callback(int param){
    if(param == id){
        clickCallback();
    }
}

int ZButton::getId(){
    return id;
}

HWND createButton(HWND parent, HINSTANCE instance, HMENU id, ZString text, int x, int y, int width, int height){
    HWND hWndButton = CreateWindowEx(0, L"BUTTON", text.wc(), WS_CHILD | WS_VISIBLE | WS_TABSTOP, x, y, width, height, parent, id, instance, NULL);
    SendMessage(hWndButton, WM_SETFONT, (WPARAM)GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(FALSE, 0));
    return hWndButton;
}

}
