#include "database.h"
#include <stdio.h>
#include <fstream>
#include <iostream>

namespace LibChaos {

Database::Database(){}

Database::Database(ZString file){
    sqlite3_open(file.cc(), &db);
}

void Database::open(ZString file){
    if(ok())
        close();
    sqlite3_open(file.cc(), &db);
}

void Database::close(){
    sqlite3_close(db);
}

bool Database::ok(){
    return (db != 0);
}

int Database::sel_callback(int num_fields, char **p_fields, char **p_col_names){
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
    Database *dbptr = (Database*) p_data;
    return dbptr->sel_callback(num_fields, p_fields, p_col_names);
}

int Database::select_rows(){
    return select_rows(query_buffer);
}
int Database::select_rows(ZString stmt){
    char *errmsg = NULL;
    format = 0;
    tmp_record_id = 0;
    records = SqlRows();
    sqlite3_exec(db, stmt.cc(), select_callback, this, &errmsg);
    if(errmsg != NULL){
        last_error = errmsg;
    }
    row_count = tmp_record_id;
    return tmp_record_id;
}
int Database::select_cols(){
    return select_cols(query_buffer);
}
int Database::select_cols(ZString stmt){
    char *errmsg = NULL;
    format = 1;
    tmp_record_id = 0;
    records = SqlRows();
    sqlite3_exec(db, stmt.cc(), select_callback, this, &errmsg);
    if(errmsg != NULL){
        last_error = errmsg;
    }
    row_count = tmp_record_id;
    return tmp_record_id;
}

bool Database::sql_stmt(ZString stmt){
    char *errmsg = NULL;
    int ret;
    ret = sqlite3_exec(db, stmt.cc(), 0, 0, &errmsg);
    if(errmsg != NULL){
        std::string tmp = errmsg;
        last_error = tmp;
    }
    row_count = 0;
    return (ret == SQLITE_OK);
}

ZString Database::lastError(){
    return last_error;
}
SqlRows Database::result(){
    return records;
}

Database &Database::operator<<(ZString query_part){
    query_buffer << query_part;
    return *this;
}

ZString Database::get_query(){
    return query_buffer;
}

void Database::clear_query(){
    query_buffer.clear();
}

int Database::rows(){
    return row_count;
}

}
