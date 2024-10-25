/**
 * @file fileSchema.h 
 * @brief Модуль, содержащий описание модели файла в проекте
 */
#ifndef FILESCHEMA_H_INCLUDED
#define FILESCHEMA_H_INCLUDED
#include <string>

using namespace std;

/** 
 * @brief Структура модели файла в проекте
 */
struct File
{
    string name; ///< Имя файла
    string path; ///< Глобальный путь к файлу
    string hash_sha1; ///< Контрольная сумма, вычисленная чрез алгоритм sha1
    string hash_sha256; ///< Контрольная сумма, вычисленная чрез алгоритм sha256
    string hash_md5; ///< Контрольная сумма, вычисленная чрез алгоритм md5
    string hash_crc32; ///< Контрольная сумма, вычисленная чрез алгоритм crc32
    bool Is_nsrl_db; ///< Переменная, указывающая принадлежность к базе NSRL
};

typedef File *FilePtr; ///< Указатель на структуру

#endif