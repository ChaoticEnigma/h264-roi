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
    rc = sqlite3_open_v2(file.str().cc(), &_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
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

int ZDatabase::execute(ZString sql){
    if(!ok())
        return -1;

    sqlite3_stmt *stmt;
    const char *tail;
    // Prepare
    rc = sqlite3_prepare_v2(_db, sql.cc(), sql.size(), &stmt, &tail);
    if(rc != SQLITE_OK)
        return -2;

    // Step rows and get column values
    while(true){
        rc = sqlite3_step(stmt);
        if(rc != SQLITE_ROW)
            break;
        int count = sqlite3_column_count();
        for(int i = 0; i < count; ++i){
            const char *text = sqlite3_column_text(stmt, i);
        }
    }
    if(rc != SQLITE_DONE)
        return -3;

    // Finalize
    sqlite3_finalize(stmt);
}

int ZDatabase::sel_callback(int num_fields, char **p_fields, char **p_col_names){
    if(format == 0){
        AsArZ tmp;
        for(int i = 0; i < num_fields; ++i){
            tmp[p_col_names[i]] = p_fields[i];
            //records[tmp_record_id][p_col_names[i]] = p_fields[i];
        }
        records.push(tmp);
    } else if(format == 1){
        for(int i = 0; i < num_fields; ++i){
            //records[p_col_names[i]][tmp_record_id] = p_fields[i];
            records[p_col_names[i]].push(p_fields[i]);
        }
    } else {
        return 1;
    }
    ++tmp_record_id;
    return 0;
}
static int select_callback(void *p_data, int num_fields, char **p_fields, char **p_col_names){
    ZDatabase *dbptr = (ZDatabase*) p_data;
    return dbptr->sel_callback(num_fields, p_fields, p_col_names);
}

int ZDatabase::select_rows(){
    return select_rows(query_buffer);
}
int ZDatabase::select_rows(ZString stmt){
    char *errmsg = NULL;
    format = 0;
    tmp_record_id = 0;
    records = SqlRows();
    sqlite3_exec(_db, stmt.cc(), select_callback, this, &errmsg);
    if(errmsg != NULL){
        last_error = errmsg;
    }
    row_count = tmp_record_id;
    return tmp_record_id;
}

int ZDatabase::select_cols(){
    return select_cols(query_buffer);
}
int ZDatabase::select_cols(ZString stmt){
    char *errmsg = NULL;
    format = 1;
    tmp_record_id = 0;
    records = SqlRows();
    sqlite3_exec(_db, stmt.cc(), select_callback, this, &errmsg);
    if(errmsg != NULL){
        last_error = errmsg;
    }
    row_count = tmp_record_id;
    return tmp_record_id;
}

bool ZDatabase::sql_stmt(ZString stmt){
    char *errmsg = NULL;
    int ret;
    ret = sqlite3_exec(_db, stmt.cc(), 0, 0, &errmsg);
    if(errmsg != NULL){
        std::string tmp = errmsg;
        last_error = tmp;
    }
    row_count = 0;
    return (ret == SQLITE_OK);
}

ZString ZDatabase::lastError(){
    return last_error;
}
SqlRows ZDatabase::result(){
    return records;
}

ZDatabase &ZDatabase::operator<<(ZString query_part){
    query_buffer << query_part;
    return *this;
}

ZString ZDatabase::get_query(){
    return query_buffer;
}

void ZDatabase::clear_query(){
    query_buffer.clear();
}

int ZDatabase::rows(){
    return row_count;
}

}
