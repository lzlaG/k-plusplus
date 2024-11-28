#include "application.h"

#include <iostream>
#include <string>
#include <vector>
#include <future>
#include <algorithm>
#include <thread>

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

void Application::GetMultiHashes(vector<FilePtr>& files, int start, int end, NSRLRepository& nsrlRepo, OutputDB& ourDatabase) {
    for (int i = start; i < end; ++i) {
        CalculateSHA1Hash(files[i]);         // Подсчет хэша
        nsrlRepo.IsHashInDB(files[i]);      // Проверка в базе NSRL
        cout << "Insert file number: " << i << " into output db" << endl;
        ourDatabase.FillTheDB(files[i]); // Заполнение базы данных
    }
}

Application::Application(int argc, const char **argv)
{
    // Добавляем пункты меню
    m_desc.add_options()
            ("help,h", "Вывести справку")
            ("nsrl-db-path,r", po::value<filesystem::path>(&m_inputDBPath)->composing(), "задать путь до базы nsrl")
            ("scan-dir,s", po::value<std::string>(&m_scanDirPath)->composing(), "задать папку для сканирования")
            ("output-db-path,o", po::value<filesystem::path>(&m_outputDBPath)->composing(), "задать путь до итоговой бд");
            //("output-db-name,n", po::value<std::string>(&m_outputDBName)->composing(), "задать название для базы ответа");
    po::store(po::parse_command_line(argc, argv, m_desc), m_vm); // парсим переданные аргументы
    po::notify(m_vm);   // записываем аргументы в переменные в программу

    //cout << "NSRL DB Path: " << m_inputDBPath << endl;
    //cout << "Scan Dir: " << m_scanDirPath << endl;
    //cout << "Output DB Path: " << m_outputDBPath << endl;
    //cout << "Output DB Name: " << m_outputDBName << endl;
}

wstring options_to_wstring(const po::options_description& desc) {
    ostringstream oss;
    oss << desc; // Описание параметров записывается в строковый поток
    return wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(oss.str());
}

/**
 * Запуск приложения
 */
int Application::exec()
{
    setlocale(LC_ALL, "");
    //переменные для записи результатов проверки
    bool nsrl_check = true;
    bool scandir_check = true;
    bool outputdb_check = true;

    // Если есть запрос на справку
    if (m_vm.count("help"))
    {
        // То выводим описание меню
        wcout << options_to_wstring(m_desc) << endl;
        return 1;
    }

    if (!m_vm.count("scan-dir"))
    {
        wcout << L"Введите параметр scan-dir" << endl;
        scandir_check = false;
        return -1;
    }

    if (!m_vm.count("nsrl-db-path"))
    {
        wcout << L"Параметр nsrl-db-path не введён." << endl;
        nsrl_check = false;
        return -1;
    }

    if (!m_vm.count("output-db-path"))
    {
        wcout << L"Параметр output-db-path не введён." << endl;
        outputdb_check = false;
        return -1;
    }

    if (scandir_check == true && nsrl_check == true && outputdb_check == true)
    {
        filesystem::path CorrectPath = m_scanDirPath;
        vector<FilePtr> filename = getFileFromDir(CorrectPath);               // Рекурсивный обход указанной директории
        cout << "Amount of files in scan dir: " << filename.size() << endl;
        NSRLRepository nsrlRepo = NSRLRepository(m_inputDBPath.string());                // Инициализация NSRL репозитория
        OutputDB ourDatabase = OutputDB(m_outputDBPath.string()); // Создание выходной базы данных

        int j = 0;

        int numThreads = std::thread::hardware_concurrency()-2;; // Число потоков
        int totalFiles = filename.size(); // количество файлов
        int filesPerThread = totalFiles / numThreads; //количество файлов отправляемых в один поток
        vector<thread> threads;

        for (int t = 0; t < numThreads; ++t) {
                int start = t * filesPerThread;
                int end = (t == numThreads - 1) ? totalFiles : start + filesPerThread;
                cout << "Thread " << t << " start: " << start << " End: " << end << endl;
                threads.emplace_back(GetMultiHashes, ref(filename), start, end, ref(nsrlRepo), ref(ourDatabase));
        }

        // Ожидание завершения всех потоков
        for (auto& t : threads) {
            t.join();
        }

        filename.clear();
        return 0;
    }
    else
    {
        return -1;
    }
}
