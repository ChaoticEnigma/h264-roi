/*******************************************************************************
**                                  LibChaos                                  **
**                                 database.h                                 **
**                          (c) 2015 Charlie Waters                           **
*******************************************************************************/
#ifndef DATABASE_H
#define DATABASE_H

#include "zstring.h"
#include "zpath.h"
#include "ztable.h"
#include "zassoc.h"

#define SQLITE_CUSTOM
#include "lib/sqlite3.h"

namespace LibChaos {

//! SQLite3 interface wrapper class.
class ZDatabase {
public:
    class Prepared;

public:
    ZDatabase();
    ZDatabase(ZPath file);
    ~ZDatabase();

    //! Open database \p file.
    bool open(ZPath file);
    //! Close database.
    void close();
    //! Check if database is open.
    bool ok();

    Prepared prepare(ZString sql);

    //! Execute an SQL query.
    int execute(ZString sql, ZTable &result);

    sqlite3 *handle(){ return _db; }

public:
    //! Prepared statement wrapper.
    class Prepared {
        friend class ZDatabase;
    private:
        Prepared(sqlite3_stmt *stmt);

    public:
        ~Prepared();
        //! Bind a value to a named SQL parameter.
        int bind(ZString name, ZString value);
        //! Execute the prepared statement.
        int execute(ZTable &result);

    private:
        sqlite3_stmt *_stmt;
    };

private:
    sqlite3 *_db;
};

typedef ZDatabase ZDB;

} // namespace LibChaos

#endif // DATABASE_H
