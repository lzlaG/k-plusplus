#include "application.h"

#include <iostream>
#include <string>
#include <vector>
#include <future>

#include "../nsrlRepository/nsrlRepository.h"
#include "../getFileFromDir/getFileFromDir.h"
#include "../OutputDB/outputDB.h"
#include "../calculateShaHash/calculateShaHash.h"
#include "../../models/fileSchema.h"

using namespace std;

/**
 * Конструктор приложения
 * @param [in] argc количество отправленных параметров
 * @param [in] argv отправленные параметры
 */
Application::Application(int argc, const char **argv)
{
    // Добавляем пункты меню
    m_desc.add_options()
            ("help,h", "Вывести справку")
            ("nsrl-db-path,r", po::value<filesystem::path>(&m_inputDBPath)->composing(), "задать путь до базы nsrl")
            ("scan-dir,s", po::value<filesystem::path>(&m_scanDirPath)->composing(), "задать папку для сканирования")
            ("output-db-path,o", po::value<filesystem::path>(&m_outputDBPath)->composing(), "задать путь до база ответа");
            //("output-db-name,n", po::value<std::string>(&m_outputDBName)->composing(), "задать название для базы ответа");
    po::store(po::parse_command_line(argc, argv, m_desc), m_vm); // парсим переданные аргументы
    po::notify(m_vm);   // записываем аргументы в переменные в программу

    cout << "NSRL DB Path: " << m_inputDBPath << endl;
    cout << "Scan Dir: " << m_scanDirPath << endl;
    cout << "Output DB Path: " << m_outputDBPath << endl;
    //cout << "Output DB Name: " << m_outputDBName << endl;
}

/**
 * Запуск приложения
 */
int Application::exec()
{
    // Если есть запрос на справку
    if (m_vm.count("help"))
    {
        // То выводим описание меню
        cout << m_desc << endl;
        return 1;
    }

    if (!m_vm.count("scan-dir"))
    {
        // То выводим описание меню
        cout << "Введите параметр scan-dir" << endl;
        return -1;
    }

    if (!m_vm.count("nsrl-db-path"))
    {
        cout << "\033[93m" << "Параметр nsrl-db-path не введён." << endl;
        //m_inputDBPath = "../src/nsrlRepository/test.db";
    }

    vector<FilePtr> filename = getFileFromDir(m_scanDirPath);               // Рекурсивный обход указанной директории
    NSRLRepository nsrlRepo = NSRLRepository(m_inputDBPath);                // Инициализация NSRL репозитория
    OutputDB ourDatabase = OutputDB(m_outputDBPath); // Создание выходной базы данных

    int j = 0;
    for (int i = 0; i < filename.size(); i++)
    {

        future<void> a1 = async([filename, i]                         // Анализ контрольной суммы файла
                                { CalculateSHA1Hash(filename[i]); }); // Подсчет хеша
        a1.wait();
        future<void> a2 = async([&nsrlRepo, filename, i]
                                { nsrlRepo.IsHashInDB(filename[i]); });
        a2.wait();
        ourDatabase.FillTheDB(filename[i]); // Заполнение баз данных
    }

    filename.clear();
    return 0;
}
