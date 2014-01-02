#include "../window.h"
#include "../control.h"

namespace LibChaosUI {

LRESULT CALLBACK ZWindow::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
#if 0
    return wndProc(hwnd, msg, wParam, lParam);
#else
    switch(msg){
        case WM_GETMINMAXINFO: {
            //MINMAXINFO *minMax = (MINMAXINFO *)lParam;
            //minMax->ptMinTrackSize.x = 400;
            //minMax->ptMinTrackSize.y = 300;
            return 0;
        }
        case WM_NCCREATE: {
            LPCREATESTRUCT cs = (LPCREATESTRUCT)lParam;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
            break;
        }
        case WM_DESTROY: {
            LOG("Destroy");
            PostQuitMessage(0);
            return 0;
        }
    }

    ZWindow *window = (ZWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if(window){
        switch(msg){
            case WM_CREATE: {
                window->setHandle(hwnd);
                ZArray<ZControl *> controls = window->getControls();
                for(zu64 i = 0; i < controls.size(); ++i){
                    if(!controls[i]->create()){
                        SLOG("Create failed.");
                    }
                    controls[i]->setChanged(false);
                }
                break;
            }
            case WM_PAINT: {
                if(window->needsUpdate()){
                    ZArray<ZControl *> controls = window->getControls();
                    for(zu64 i = 0; i < controls.size(); ++i){
                        if(controls[i]->isChanged()){
                            if(!controls[i]->update()){
                                SLOG("Update failed.");
                            }
                            controls[i]->setChanged(false);
                        }
                    }
                    window->setNeedUpdate(false);
                }
                break;
            }

            case WM_COMMAND: {
                WORD param = LOWORD(wParam);

                ZWindow::ControlMap *handlers = window->getHandlers();
//                ZUIWindow::ControlMap::iterator it = handlers->find(param);
//                if(it != handlers->end()){
//                   it.second;
//                }

                if(handlers->count(param)){
                    handlers->at(param)->callback(param);
                }
                break;
            }
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
#endif
}

} // namespace LibChaosUI
