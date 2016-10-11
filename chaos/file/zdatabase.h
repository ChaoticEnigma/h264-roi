/*******************************************************************************
**                                  LibChaos                                  **
**                                 database.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef DATABASE_H
#define DATABASE_H

#include "zstring.h"
#include "zpath.h"
#include "zassoc.h"

#define SQLITE_CUSTOM
#include "lib/sqlite3.h"

namespace LibChaos {

typedef ZAssoc<ZString, AsArZ> SqlRows;

//! SQLite3 interface wrapper class.
class ZDatabase {
public:
    ZDatabase();
    ZDatabase(ZPath file);
    ~ZDatabase();

    bool open(ZPath file);
    void close();
    bool ok();

    int execute(ZString sql);

    int select_rows();
    int select_rows(ZString stmt);
    int select_cols();
    int select_cols(ZString stmt);
    bool sql_stmt(ZString stmt);

    SqlRows result();
    int rows();

    ZString lastError();
    ZDatabase &operator<<(ZString query_part);
    ZString get_query();
    void clear_query();

    sqlite3 *handle(){ return _db; }

public:
    int sel_callback(int num_fields, char **p_fields, char **p_col_names); // For internal use

private:
    sqlite3 *_db;
    int rc;
    ZString last_error;
    ZString query_buffer;
    short format;

    int row_count;
    SqlRows records;
    int tmp_record_id;
};

typedef ZDatabase ZDB;

} // namespace LibChaos

#endif // DATABASE_H
