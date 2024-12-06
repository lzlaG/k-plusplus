/**
 * @file getFileFromDir.h
 * @brief Модуль, необходимый для получения имен и путей файлов из указанной директории
 */
#ifndef DIR
#define DIR
#include <vector>
#include "../../models/fileSchema.h"
#include <filesystem>
using namespace std;

/**
 * Фунция обхода директории
 */
vector<FilePtr> getFileFromDir(filesystem::path path);
#endif // DIR
