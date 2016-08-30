#ifndef ZTABLE_H
#define ZTABLE_H

#include "zarray.h"
#include "zmap.h"

namespace LibChaos {

class ZTable {
public:
    ZTable();

    //! Get reference to field in \a column and \a row.
    ZString &field(ZString column, zu64 row);
    //! Get reference to record in \a row.
    ZArray<ZString> &record(zu64 row);

    //! Add a column to the table.
    void addColumn(ZString name);
    //! Adda record to the table.
    void addRecord(ZArray<ZString> record);

    //! Get an array of fields in \a column.
    ZArray<ZString> getColumn(ZString name);
    //! Get a map of column names to fields in \a row.
    ZMap<ZString> getRecord(zu64 row);

private:
    ZMap<ZString, zu64> _columns;
    ZArray<ZArray<ZString>> _records;
};

}

#endif // ZTABLE_H
