#include "../align.h"

namespace LibChaosUI {

ZAlign::ZAlign(){}

void ZAlign::add(ZControl *control){
    control->init(this);
    control->setChanged(true);
    controls.push(control);
    return;
}

bool ZAlign::create(){
    parent->setHandle(hwnd);
    ZArray<ZControl *> controls = parent->getControls();
    for(zu64 i = 0; i < controls.size(); ++i){
        if(!controls[i]->create()){
            SLOG("Create failed.");
        }
        controls[i]->setChanged(false);
    }
}

bool ZAlign::update(){
    if(parent->needsUpdate()){
        ZArray<ZControl *> controls = parent->getControls();
        for(zu64 i = 0; i < controls.size(); ++i){
            if(controls[i]->isChanged()){
                if(!controls[i]->update()){
                    SLOG("Update failed.");
                }
                controls[i]->setChanged(false);
            }
        }
        parent->setNeedUpdate(false);
    }
}

} // namespace LibChaosUI
