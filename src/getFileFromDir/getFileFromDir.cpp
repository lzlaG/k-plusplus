#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;
using namespace filesystem;

#include "../../models/fileSchema.h"
#include "getFileFromDir.h"

/**
 * @param [in] path Путь к анализируемой директории
 * @param [out] result Заполнены вектор со считанными именами и путями в собственных структурах для каждого файла
 */
void getFilesFromDirRecursive(const filesystem::path &path, vector<FilePtr> &result)
{
    try
    {
        for (const auto &dirEntry : directory_iterator(path, directory_options::skip_permission_denied))
        {
            try
            {
                cout << "File: " << dirEntry.path() << endl;
                if (dirEntry.is_symlink())
                    continue; // Игнорируем символические ссылки

                const auto filename = dirEntry.path().filename();
                if (filename == "System Volume Information" || filename == "$Recycle.Bin")
                    continue; // Игнорируем системные папки

                if (dirEntry.is_regular_file())
                {
                    auto file = new File();
                    file->path = dirEntry.path().string();
                    file->name = dirEntry.path().filename().string();
                    result.push_back(file);
                }
                else if (dirEntry.is_directory())
                {
                    // Рекурсивно обходим вложенные директории
                    getFilesFromDirRecursive(dirEntry.path(), result);
                }
            }
            catch (const filesystem_error &e)
            {
                cerr << "Filesystem error: " << e.what() << " at path: "
                     << (e.path1().empty() ? "(unknown)" : e.path1().string()) << endl;
            }
        }
    }
    catch (const filesystem_error &e)
    {
        cerr << "filesystem error(directory): " << path << ": " << e.what() << endl;
    }
}

vector<FilePtr> getFileFromDir(filesystem::path path)
{
    vector<FilePtr> result;
    getFilesFromDirRecursive(path, result);
    return result;
}
