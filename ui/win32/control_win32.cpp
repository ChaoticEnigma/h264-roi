#include "../control.h"
#include "../window.h"

namespace LibChaosUI {

ZControl::ZControl() : parent(NULL), changed(false){}
ZControl::~ZControl(){}

void ZControl::init(ZWindow *par){
    parent = par;
    setChanged(true);
    initialize();
}
void ZControl::initialize(){
    return;
}

bool ZControl::create(){
    LOG("ZUIControl create called directly.");
    return false;
}
bool ZControl::update(){
    return create();
}

void ZControl::callback(int){
    return;
}

ZWindow *ZControl::getParent(){
    return parent;
}
bool ZControl::isChanged(){
    return changed;
}

void ZControl::setChanged(bool change){
    changed = change;
    if(changed)
        parent->setNeedUpdate(true);
}

}
