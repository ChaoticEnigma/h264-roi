#include "zdatabase.h"

#include <stdio.h>
#include <fstream>

namespace LibChaos {

ZDatabase::ZDatabase() : _db(NULL){}

ZDatabase::ZDatabase(ZPath file) : ZDatabase(){
    open(file);
}

ZDatabase::~ZDatabase(){
    close();
}

bool ZDatabase::open(ZPath file){
    if(ok())
        close();
    int rc = sqlite3_open_v2(file.str().cc(), &_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if(rc != SQLITE_OK)
        return false;
    return true;
}

void ZDatabase::close(){
    sqlite3_close_v2(_db);
    _db = NULL;
}

bool ZDatabase::ok(){
    return (_db != NULL);
}

ZDatabase::Prepared ZDatabase::prepare(ZString sql){
    if(!ok())
        return Prepared(NULL);

    sqlite3_stmt *stmt;
    const char *tail;

    // Prepare
    int rc = sqlite3_prepare_v2(_db, sql.cc(), sql.size(), &stmt, &tail);
    if(rc != SQLITE_OK)
        return Prepared(NULL);
    return Prepared(stmt);
}

int ZDatabase::execute(ZString sql, ZTable &result){
    if(!ok())
        return -1;

    Prepared prep = prepare(sql);
    return prep.execute(result);
}

// /////////////////////////////////////////////////////////////////////////////

ZDatabase::Prepared::Prepared(sqlite3_stmt *stmt) : _stmt(stmt){

}

ZDatabase::Prepared::~Prepared(){
    // Finalize
    sqlite3_finalize(_stmt);
}

int ZDatabase::Prepared::bind(ZString name, ZString value){
    int index = sqlite3_bind_parameter_index(_stmt, name.cc());
    int ret = sqlite3_bind_text(_stmt, index, value.cc(), value.size(), SQLITE_TRANSIENT);
    return ret;
}

int ZDatabase::Prepared::execute(ZTable &result){

    // Step rows and get column values
    int rc;
    while(true){
        rc = sqlite3_step(_stmt);
        if(rc != SQLITE_ROW)
            break;
        int count = sqlite3_column_count(_stmt);
        ZArray<ZString> record;
        for(int i = 0; i < count; ++i){
            const char *name = sqlite3_column_name(_stmt, i);
            result.addColumn(name);
            const unsigned char *text = sqlite3_column_text(_stmt, i);
            record.push(text);
        }
        result.addRecord(record);
    }
    if(rc != SQLITE_DONE)
        return -3;

    return 0;
}

}
