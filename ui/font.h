#ifndef ZUIFONT_H
#define ZUIFONT_H

#include "base.h"

namespace LibChaosUI {

class ZFont {
public:
    ZFont();
    ZFont(ZString name, int height, bool italic = false);
    ~ZFont();

    HFONT handle();
private:
    HFONT font;
};

}

#endif // ZUIFONT_H
