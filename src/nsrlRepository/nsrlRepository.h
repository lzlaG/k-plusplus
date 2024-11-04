/**
 * @file nsrlRepository.h
 * @brief Модуль, необходимый для взаимодействия с базой NSRL
 */
#include "../../lib/sqlite3/sqlite3.h"
#ifndef NSRLREPO_H_INCLUDED
#define NSRLREPO_H_INCLUDED

#include "../../models/fileSchema.h"

/**
 * Класс для взаимодействия с NSRL
 */
class NSRLRepository
{
private:
    sqlite3 *Database;

    static int callback(void *count, int argc, char **argv, char **azColName)
    {
        int *c = (int *)count;
        *c = atoi(argv[0]);
        return 0;
    }

public:
    class OpenDBException // Для проброса ошибок открытия БД
    {
    public:
        int error;
        OpenDBException(int Error) { error = Error; };
    };
    NSRLRepository(string path);
    ~NSRLRepository() { sqlite3_close(Database); }; // закрытие бд
    /**
     * Функция считывания хэша из базы NSRL
     */
    void IsHashInDB(FilePtr file);
};

typedef NSRLRepository *NSRLRepositoryPtr; ///< указатель на репозиторий

#endif // NSRLREPO_H_INCLUDED