#include "ztable.h"

namespace LibChaos {

ZTable::ZTable(){

}

ZString &ZTable::field(ZString column, zu64 row){
    return _records[row][_columns[column]];
}

ZArray<ZString> &ZTable::record(zu64 row){
    return _records[row];
}

void ZTable::addColumn(ZString name){
    _columns[name] = _columns.size();
    for(zu64 i = 0; i < _records.size(); ++i){
        _records[i].resize(_columns.size());
    }
}

void ZTable::addRecord(ZArray<ZString> record){
    if(record.size() != _columns.size())
        throw ZException("Incorrect number of fields in record");
    _records.push(record);
}

ZArray<ZString> ZTable::getColumn(ZString name){
    ZArray<ZString> column;
    for(zu64 i = 0; i < _records.size(); ++i){
        column.push(_records[i][_columns[name]]);
    }
    return column;
}

ZMap<ZString, ZString> ZTable::getRecord(zu64 row){
    ZMap<ZString, ZString> map;
    for(auto it = _columns.begin(); it.more(); ++it){
        map[*it] = _records[row][_columns[*it]];
    }
    return map;
}

}
