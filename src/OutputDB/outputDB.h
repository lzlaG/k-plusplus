/**
 * @file outputDB.h
 * @brief Модуль, необходимый для создания и заполнения пораждаемых бах данных
 */
#ifndef OUTPUT_DB_H
#define OUTPUT_DB_H

#include "../../lib/sqlite3/sqlite3.h"
#include "../../models/fileSchema.h"
#include <iostream>

using namespace std;

/**
 * Класс для работы с пораждаемыми базами данных
 */
class OutputDB
{
private:
    sqlite3 *DB;

public:
    OutputDB(string path)
    {
        sqlite3_open((path).c_str(), &DB);
        string create_known_table = "CREATE TABLE IF NOT EXISTS KNOWN_FILES ("
                                    "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                    "NAME TEXT,"
                                    "PATH TEXT,"
                                    "HASH TEXT"
                                    ");";
        string create_unknown_table = "CREATE TABLE IF NOT EXISTS UNKNOWN_FILES ("
                                      "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                                      "NAME TEXT,"
                                      "PATH TEXT,"
                                      "HASH TEXT"
                                      ");";
        string delete_known_table = "DELETE FROM KNOWN_FILES;";
        string delete_unknown_table = "DELETE FROM UNKNOWN_FILES;";
        char *errMsg;
        sqlite3_exec(DB, create_known_table.c_str(), nullptr, nullptr, &errMsg);
        sqlite3_exec(DB, create_unknown_table.c_str(), nullptr, nullptr, &errMsg);
        sqlite3_exec(DB, delete_known_table.c_str(), nullptr, nullptr, &errMsg);
        sqlite3_exec(DB, delete_unknown_table.c_str(), nullptr, nullptr, &errMsg);
        cout << errMsg << "\n";
    };
    void FillTheDB(FilePtr ourfile);
};
#endif // OUTPUT_DB_H