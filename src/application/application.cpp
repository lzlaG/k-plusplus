#include "application.h"

#include <iostream>
#include <string>
#include <vector>

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
    m_desc.add_options()("help,h", "Вывести справку")("nsrl-db-path,n", po::value<filesystem::path>(&m_inputDBPath)->composing(), "задать путь до базы nsrl")("scan-dir,s", po::value<filesystem::path>(&m_scanDirPath)->composing(), "задать папку для сканирования")("output-db-path,o", po::value<filesystem::path>(&m_outputDBPath)->composing(), "задать путь до база ответа");
    po::store(po::parse_command_line(argc, argv, m_desc), m_vm); // парсим переданные аргументы
    po::notify(m_vm);                                            // записываем аргументы в переменные в программе
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
        cout << "\033[93m" << "Параметр nsrl-db-path не введён." << "\033[0m" << " Используется не полная тестовая бд nsrl" << endl;
        m_inputDBPath = "../src/nsrlRepository/test.db";
    }

    if (!m_vm.count("output-db-path"))
    {
        m_outputDBPath = "output.db";
    }

    vector<FilePtr> filename = getFileFromDir(m_scanDirPath); // Рекурсивный обход указанной директории
    NSRLRepository nsrlRepo = NSRLRepository(m_inputDBPath);  // Инициализация NSRL репозитория
    OutputDB ourDatabase = OutputDB(m_outputDBPath);          // Создание выходных баз данных

    int j = 0;
    for (int i = 0; i < filename.size(); i++)
    {
        CalculateSHA1Hash(filename[i]);     // Подсчет хеша
        nsrlRepo.IsHashInDB(filename[i]);   // Анализ контрольной суммы файла
        ourDatabase.FillTheDB(filename[i]); // Заполнение баз данных
        if (i > j * 10)
        {
            cout << "Обработана" << j << " сотка" << endl;
            j++;
        }
    }

    filename.clear();
    return 0;
}