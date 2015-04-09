/*******************************************************************************
**                                  LibChaos                                  **
**                                 database.h                                 **
**                          (c) 2015 Zennix Studios                           **
*******************************************************************************/
#ifndef DATABASE_H
#define DATABASE_H

#include "zstring.h"
#include "zassoc.h"

#define SQLITE_CUSTOM
#include "lib/sqlite3.h"

namespace LibChaos {

typedef ZAssoc<ZString, AsArZ> SqlRows;

class Database {
public:
    Database();
    Database(ZString file);
    ~Database();

    void open(ZString file);
    bool ok();
    void close();

    int select_rows();
    int select_rows(ZString stmt);
    int select_cols();
    int select_cols(ZString stmt);
    bool sql_stmt(ZString stmt);

    SqlRows result();
    int rows();

    ZString lastError();
    Database &operator<<(ZString query_part);
    ZString get_query();
    void clear_query();

    sqlite3 *raw(){ return db; }

    int sel_callback(int num_fields, char **p_fields, char **p_col_names); // For internal use
private:
    sqlite3 *db;
    int row_count;
    SqlRows records;
    ZString last_error;
    short format;
    int tmp_record_id;
    ZString query_buffer;
    int rc;
};

typedef Database ZDatabase;
typedef Database ZDB;

} // namespace LibChaos

#endif // DATABASE_H
