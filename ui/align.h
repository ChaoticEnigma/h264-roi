#ifndef ALIGN_H
#define ALIGN_H

#include "control.h"

namespace LibChaosUI {

class ZAlign : public ZControl {
public:
    ZAlign();
    void add(ZControl *control);
    bool create();
    bool update();

private:
    ZArray<ZControl *> controls;
};

} // namespace LibChaosUI

#endif // ALIGN_H
