/** 
 * @file calculateShaHash.h
 * @brief Модуль, необходимый для подсчета контрольной суммы файла алгоритмом sha1
 */
#ifndef CalculateSHA1Hash_H_INCLUDED
#define CalculateSHA1Hash_H_INCLUDED
#include "../../models/fileSchema.h"

using namespace std;

/** 
 * Функция подсчета хэша
 * @param [in] file Структура файла с названием и путем до него
 */
void CalculateSHA1Hash(FilePtr file);

#endif // CalculateSHA1Hash_H_INCLUDED