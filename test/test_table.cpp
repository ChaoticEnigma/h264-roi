#include "tests.h"
#include "ztable.h"

namespace LibChaosTest {

void table(){
    ZTable table1;
    table1.addColumn("col1");
    table1.addColumn("col2");
    table1.addColumn("col3");
    table1.addColumn("col4");

    table1.addRecord({ "val1", "val2", "val3", "val4" });
    table1.addRecord({ "val5", "val6", "val7", "val8" });

    TASSERT(table1.field("col1", 0) == "val1");
    TASSERT(table1.field("col2", 0) == "val2");
    TASSERT(table1.field("col3", 0) == "val3");
    TASSERT(table1.field("col4", 0) == "val4");
    TASSERT(table1.field("col1", 1) == "val5");
    TASSERT(table1.field("col2", 1) == "val6");
    TASSERT(table1.field("col3", 1) == "val7");
    TASSERT(table1.field("col4", 1) == "val8");
}

ZArray<Test> table_tests(){
    return {
        { "table",   table,   true, { "array", "map" } },
    };
}

}
