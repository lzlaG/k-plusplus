#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <thread>
#include <mutex>
#include "../../models/fileSchema.h"
#include "getFileFromDir.h"

using namespace std;
using namespace filesystem;

mutex resultMutex; // Мьютекс для синхронизации доступа к result

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

                    // Защита добавления в result с помощью мьютекса
                    lock_guard<mutex> lock(resultMutex);
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

void processDirectories(const vector<path> &paths, vector<FilePtr> &result)
{
    for (const auto &subPath : paths)
    {
        getFilesFromDirRecursive(subPath, result);
    }
}

vector<FilePtr> getFileFromDir(filesystem::path path)
{
    vector<FilePtr> result;

    vector<filesystem::path> subDirectories;
    try
    {
        for (const auto &dirEntry : directory_iterator(path, directory_options::skip_permission_denied))
        {
            if (dirEntry.is_directory())
            {
                subDirectories.push_back(dirEntry.path());
            }
            else if (dirEntry.is_regular_file())
            {
                auto file = new File();
                file->path = dirEntry.path().string();
                file->name = dirEntry.path().filename().string();
                result.push_back(file);
            }
        }
    }
    catch (const filesystem_error &e)
    {
        cerr << "filesystem error: " << path << ": " << e.what() << endl;
    }

    // Разделение поддиректорий на три части
    size_t numThreads = 3;
    vector<thread> threads;
    vector<filesystem::path> directories[numThreads];

    for (size_t i = 0; i < subDirectories.size(); ++i)
    {
        directories[i % numThreads].push_back(subDirectories[i]);
    }

    // Запускаем потоки
    for (size_t i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(processDirectories, ref(directories[i]), ref(result));
    }

    // Ожидаем завершения потоков
    for (auto &t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    return result;
}
