#include "ztable.h"

namespace LibChaos {

ZTable::ZTable(){

}

ZTable::ZTable(ZArray<ZString> columns) : ZTable(){
    for(zu64 i = 0; i < columns.size(); ++i){
        _columns.add(columns[i], i);
    }
}

void ZTable::addColumn(ZString name){
    if(_columns.contains(name))
        return;
    zu64 num = _columns.size();
    _columns[name] = num;
    for(zu64 i = 0; i < _records.size(); ++i){
        _records[i].resize(_columns.size());
    }
}

void ZTable::addRecord(ZArray<ZString> record){
    if(record.size() != _columns.size())
        throw ZException("Incorrect number of fields in record");
    _records.push(record);
}

ZString &ZTable::field(zu64 column, zu64 row){
    return _records[row][column];
}

ZString &ZTable::field(ZString column, zu64 row){
    return _records[row][_columns[column]];
}

ZArray<ZString> &ZTable::record(zu64 row){
    return _records[row];
}

ZArray<ZString> ZTable::getColumnNames(){
    return _columns.keys();
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

zu64 ZTable::colCount(){
    return _columns.size();
}

zu64 ZTable::rowCount(){
    return _records.size();
}

}
