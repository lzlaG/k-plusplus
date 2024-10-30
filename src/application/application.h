#ifndef MENU_EAPPLICATION_H
#define MENU_EAPPLICATION_H

#include <boost/program_options.hpp>
#include <filesystem>

namespace po = boost::program_options;

using namespace std;

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
    filesystem::path m_inputDBPath;
    filesystem::path m_scanDirPath;
    filesystem::path m_outputDBPath;
};

#endif // MENU_EAPPLICATION_H