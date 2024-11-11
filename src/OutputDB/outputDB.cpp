#include <iostream>
#include "outputDB.h"

using namespace std;

/**
 * Заполнение базы данных
 * @param [in] ourfile Заполненная структура файла
 */
void OutputDB::FillTheDB(FilePtr ourfile)
{
    if (ourfile->Is_nsrl_db == true)
    {
        sqlite3_stmt *stmt;
        string sql = "INSERT INTO KNOWN_FILES (NAME, HASH, PATH) VALUES ('" + ourfile->name + "','" + ourfile->hash_sha1 + "','" + ourfile->path + "');";
        int err = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    else
    {
        sqlite3_stmt *stmt;
        string sql = "INSERT INTO UNKNOWN_FILES (NAME, HASH, PATH) VALUES ('" + ourfile->name + "', '" + ourfile->hash_sha1 + "','" + ourfile->path + "');";
        sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, NULL);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    };
};
