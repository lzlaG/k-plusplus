/** 
 * @file getFileFromDir.h
 * @brief Модуль, необходимый для получения имен и путей файлов из указанной директории
 */
#ifndef DIR
#define DIR
#include <vector>
#include "../../models/fileSchema.h"
using namespace std;

/** 
 * Фунция обхода директории
 */
vector<FilePtr> getFileFromDir(string path);
#endif // DIR
