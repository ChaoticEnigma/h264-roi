#ifndef PARCELMAIN_H
#define PARCELMAIN_H

#include "zstring.h"
#include "zpath.h"
#include "zfile.h"
#include "zjson.h"
#include "zlog.h"

#include "zparcel.h"

#include <cstdlib>

using namespace LibChaos;

ZArray<ZPath> autoFiles(ArZ args);
ZArray<ZPath> manualFiles(ArZ args);

#endif // PARCELMAIN_H
