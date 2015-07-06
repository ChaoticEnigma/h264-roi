#ifndef ZPDF_H
#define ZPDF_H

#include "ztypes.h"
#include "zstring.h"
#include "zpath.h"
#include "zfile.h"

namespace LibChaos {

class ZPDF {
public:
    ZPDF(ZPath file);
    ~ZPDF();

    void list() const;

private:
    void read();

private:
    ZFile _file;
    ZArray<ZBinary> _lines;
};

}

#endif // ZPDF_H
