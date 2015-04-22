#ifndef ZPARCELTYPES_H
#define ZPARCELTYPES_H

#include "ztypes.h"
#include "zbinary.h"

namespace LibChaos {
namespace ZParcelTypes {

typedef zu32 pageid;
enum pagetype {
    freepage,
    fieldpage,
    freelistpage,
    indexpage,
    recordpage,
    blobpage,
    historypage,
    headpage,
};

typedef zu16 fieldid;
enum fieldtype {
    nullfield,
    unsignedintfield,
    signedintfield,
    zuidfield,
    stringfield,
    filefield,
    binaryfield,
    floatfield,
};

struct Field {
    fieldid id;
    ZBinary data;
};

typedef ZArray<Field> FieldList;

}
}

#endif // ZPARCELTYPES_H

