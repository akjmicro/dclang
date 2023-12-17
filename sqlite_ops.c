#include <sqlite3.h>

// Wrapper function for sqlite3_open
void _sqliteopenfunc()
{
    const char* db_path = (const char*)(DCLANG_PTR)dclang_pop();
    sqlite3* db;
    int rc = sqlite3_open(db_path, &db);
    if (rc != SQLITE_OK) {
        printf("SQLite3 error: %s\n", sqlite3_errmsg(db));
        return;
    }
    push((DCLANG_PTR)db);
}

// Wrapper function for sqlite3_prepare
void _sqlitepreparefunc() {
    char *sql = (char *)(DCLANG_PTR)dclang_pop();
    sqlite3* db = (sqlite3 *)(DCLANG_PTR)dclang_pop();
    const char *sql_const = (const char *) sql;
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql_const, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        printf("SQLite3 error: %s\n", sqlite3_errmsg(db));
        return;
    }
    push((DCLANG_PTR)stmt);
}

// Wrapper function for sqlite3_step
void _sqlitestepfunc() {
    sqlite3_stmt* stmt = (sqlite3_stmt *)(DCLANG_PTR)dclang_pop();
    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
        printf("SQLite3 error: %s\n", sqlite3_errmsg(sqlite3_db_handle(stmt)));
        return;
    }
    push((DCLANG_PTR)rc);
}

// Wrapper function for sqlite3_column
void _sqlitecolumnfunc() {
    int col_index = (int)dclang_pop();
    sqlite3_stmt* stmt = (sqlite3_stmt *)(DCLANG_PTR)dclang_pop();
    // Get the number of columns in the result set
    int col_count = sqlite3_column_count(stmt);
    // Check if the column index is within a valid range
    if (col_index < 0 || col_index >= col_count) {
        printf("Column index out of bounds! ");
        return;
    }
    char *text = sqlite3_column_text(stmt, col_index);
    DCLANG_PTR text_ptr = (DCLANG_PTR) text;
    int charsize = strlen(text);
    // update print safety:
    if (text_ptr < MIN_STR || MIN_STR == 0)
    {
        MIN_STR = text_ptr;
    }
    if (text_ptr + charsize + 1 > MAX_STR || MAX_STR == 0)
    {
        MAX_STR = text_ptr + charsize + 1;
    }
    push(text_ptr);
}

// Wrapper function for sqlite3_finalize
void _sqlitefinalizefunc() {
    sqlite3_stmt* stmt = (sqlite3_stmt *)(DCLANG_PTR)dclang_pop();
    int rc = sqlite3_finalize(stmt);
    if (rc != SQLITE_OK) {
        printf("SQLite3 error: %s\n", sqlite3_errmsg(sqlite3_db_handle(stmt)));
        return;
    }
}

// _sqliteexecfunc is a convenience wrapper
static int __sqlcallback(void *NotUsed, int argc, char **argv, char **azColName)
{
  int i;
  for(i=0; i<argc; i++){
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void _sqliteexecfunc()
{
    char *zErrMsg = 0;
    char *sql = (char *)(DCLANG_PTR)dclang_pop();
    sqlite3* db = (sqlite3 *)(DCLANG_PTR)dclang_pop();
    int rc = sqlite3_exec(db, sql, __sqlcallback, 0, &zErrMsg);
    if(rc != SQLITE_OK) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
}

// Wrapper function for sqlite3_close
void _sqliteclosefunc() {
    sqlite3* db = (sqlite3 *)(DCLANG_PTR)dclang_pop();
    int rc = sqlite3_close(db);
    if (rc != SQLITE_OK) {
        printf("SQLite3 error: %s\n", sqlite3_errmsg(db));
        return;
    }
}
