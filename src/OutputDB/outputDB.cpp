#include <iostream>
#include "outputDB.h"
#include "../../lib/sqlite3/sqlite3.h"
#include <filesystem>

using namespace std;

string ConvertPathToUtf8(const std::filesystem::path &path) {
#ifdef _WIN32
    return path.u8string();
#else
    return path.string();
#endif
}

/**
 * Заполнение базы данных
 * @param [in] ourfile Заполненная структура файла
 */
void OutputDB::FillTheDB(FilePtr ourfile)
{
    const char *sql_known = "INSERT INTO KNOWN_FILES (NAME, HASH, PATH) VALUES (?, ?, ?);";
    const char *sql_unknown = "INSERT INTO UNKNOWN_FILES (NAME, HASH, PATH) VALUES (?, ?, ?);";

    sqlite3_stmt *stmt;
    const char *sql = ourfile->Is_nsrl_db ? sql_known : sql_unknown;

    string path_utf8 = ConvertPathToUtf8(ourfile->path);
    if (sqlite3_prepare_v2(DB, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, ourfile->name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, ourfile->hash_sha1.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, path_utf8.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::cerr << "Ошибка выполнения запроса: " << sqlite3_errmsg(DB) << std::endl;
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(DB) << std::endl;
    }
}

sqlite3* OutputDB::GetDB()
{
    return DB;
}
