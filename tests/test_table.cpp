#include "tests.h"

#include "ztable.h"

namespace LibChaosTest {

void table(){
    ZTable table;
    table.addColumn("col1");
    table.addColumn("col2");
    table.addColumn("col3");
    table.addColumn("col4");
    table.addRecord({ "val11", "val12", "val13", "val14" });
    table.addRecord({ "val21", "val22", "val23", "val24" });
    table.addRecord({ "val31", "val32", "val33", "val34" });
    table.addRecord({ "val41", "val42", "val43", "val44" });
    TASSERT(table.field("col1", 0) == "val11");
    TASSERT(table.field("col2", 0) == "val12");
    TASSERT(table.field("col3", 0) == "val13");
    TASSERT(table.field("col4", 0) == "val14");
    TASSERT(table.field("col1", 1) == "val21");
    TASSERT(table.field("col2", 1) == "val22");
    TASSERT(table.field("col3", 1) == "val23");
    TASSERT(table.field("col4", 1) == "val24");
    TASSERT(table.field("col1", 2) == "val31");
    TASSERT(table.field("col2", 2) == "val32");
    TASSERT(table.field("col3", 2) == "val33");
    TASSERT(table.field("col4", 2) == "val34");
    TASSERT(table.field("col1", 3) == "val41");
    TASSERT(table.field("col2", 3) == "val42");
    TASSERT(table.field("col3", 3) == "val43");
    TASSERT(table.field("col4", 3) == "val44");
}

ZArray<Test> table_tests(){
    return {
        { "table", table, true, { "array-initializer", "map" } },
    };
}

}
