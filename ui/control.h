#ifndef ZUICONTROL_H
#define ZUICONTROL_H

#include "base.h"

namespace LibChaosUI {

class ZWindow;

class ZControl {
public:
    ZControl();
    virtual ~ZControl();

    void init(ZWindow *par);
    virtual void initialize();
    virtual bool create();
    virtual bool update();

    virtual void callback(int);

    ZWindow *getParent();
    bool isChanged();

    void setChanged(bool ch);
protected:
    ZWindow *parent;
    bool changed;
};

}

#endif // ZUICONTROL_H
