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

} // namespace LibChaosUI

#endif // ZUIFONT_H
