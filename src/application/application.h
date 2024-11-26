/**
 * @file application.h
 * @brief Основной модуль, необходимый для парсинга параметров из командной строки
 */
#ifndef MENU_EAPPLICATION_H
#define MENU_EAPPLICATION_H

#include <boost/program_options.hpp>
#include <filesystem>
#include "../../models/fileSchema.h"
#include "../nsrlRepository/nsrlRepository.h"
#include "../OutputDB/outputDB.h"


namespace po = boost::program_options;

using namespace std;

/**
 * Класс всего приложения, содержит парсер командной строки и основную логику приложения
 */
class Application
{
public:
    explicit Application(int argc, const char **argv);
    int exec();

private:
    // Описание доступных опций меню
    po::options_description m_desc{"Allowed options"};
    po::variables_map m_vm; // контейнер для сохранения выбранных опций программы

    // Требуемые в программе переменные для работы с опциями меню
    filesystem::path m_inputDBPath;      // Путь до базы NSRL
    std::string m_scanDirPath;      // Путь для сканирования
    filesystem::path m_outputDBPath;     // Путь для создания бд для ответа
    //string m_outputDBName = "output.db"; // Название созданной бд
    static void GetMultiHashes(vector<FilePtr>& files, int start, int end, NSRLRepository& nsrlRepo,OutputDB& ourDatabase);
};

#endif // MENU_EAPPLICATION_H
