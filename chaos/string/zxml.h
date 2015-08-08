/*******************************************************************************
**                                  LibChaos                                  **
**                                   zxml.h                                   **
**                          See COPYRIGHT and LICENSE                         **
*******************************************************************************/
#ifndef ZXML_H
#define ZXML_H

#include "zstring.h"

namespace LibChaos {

class ZXML {
public:
    ZXML(ZString _xml);
private:
    ZString xml;
};

}

#endif // ZXML_H
