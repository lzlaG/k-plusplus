#include "slave.h"
#include <QString>
#include <QStandardItemModel>
#include <QCoreApplication>
#include "../../lib/sqlite3/sqlite3.h"
#include <filesystem>
#include "../nsrlRepository/nsrlRepository.h"
#include "../OutputDB/outputDB.h"
#include "../getFileFromDir/getFileFromDir.h"
#include "../calculateShaHash/calculateShaHash.h"
#include <QDateTime>
#include <thread>
#include <vector>

void Slave::GuiMultiHashes(vector<FilePtr>& files, int start, int end, NSRLRepository& nsrlRepo, OutputDB& ourDatabase) {
    for (int i = start; i < end; ++i) {
        CalculateSHA1Hash(files[i]);         // Подсчет хэша
        nsrlRepo.IsHashInDB(files[i]);      // Проверка в базе NSRL
        //cout << "Insert file number: " << i << " into output db" << endl;
        ourDatabase.FillTheDB(files[i]); // Заполнение базы данных
        //cout << "File number: " << i << endl;
        emit ProgressUpdated(1);
        //if (i % 80 == 0) {
        //    emit AnekdotTime();
        //}
    }
}

QString Slave::ReadFiles(QString NsrlFile, QString ScanDir)
{

    QString PathToDB = QCoreApplication::applicationDirPath()+
            QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss")+".db"; //уникальное имя для бд
    std::filesystem::path ScanDirCorrect = ScanDir.toStdString();
    filesystem::path CorrectPath = ScanDirCorrect;
    vector<FilePtr> filename = getFileFromDir(CorrectPath);   // Рекурсивный обход указанной директории
    cout << "Amount of files in scan dir: " << filename.size() << endl;
    NSRLRepository nsrlRepo = NSRLRepository(NsrlFile.toStdString());    // Инициализация NSRL репозитория
    OutputDB ourDatabase = OutputDB(PathToDB.toStdString());   // Создание выходной базы данных
    emit ChangeRange(filename.size()); //издаем сигнал об изменении диапазона

    int numThreads = std::thread::hardware_concurrency()-2; // Число потоков
    int totalFiles = filename.size(); // количество файлов
    emit ChangeRange(totalFiles);
    cout << "Amount of files in scan dir: " << totalFiles << endl;
    int filesPerThread = totalFiles / numThreads; //количество файлов отправляемых в один поток
    std::vector<std::thread> threads;

    for (int t = 0; t < numThreads; ++t) {
            int start = t * filesPerThread;
            int end = (t == numThreads - 1) ? totalFiles : start + filesPerThread;
            cout << "Thread " << t << " start: " << start << " End: " << end << endl;
            threads.emplace_back(&Slave::GuiMultiHashes, this, ref(filename), start, end, ref(nsrlRepo), ref(ourDatabase));
    }

    // Ожидание завершения всех потоков
    for (auto& t : threads) {
        t.join();
    }
    return PathToDB;
}

void Slave::GetDataFromDB(QString DB_path)
{
    sqlite3 *DB;
    sqlite3_open(DB_path.toUtf8().constData(), &DB);
    sqlite3_stmt* stmt;
    const char * KnownQuery = "SELECT * FROM KNOWN_FILES;";
    if (sqlite3_prepare_v2(DB, KnownQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        wcout << L"Ошибка подготовки запроса для таблицы с известными файлами:" << sqlite3_errmsg(DB);
        return;
    }
    QStandardItemModel *KnownModel= new QStandardItemModel();
    // Итерация по строкам результата
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QList<QStandardItem*> QueryResult;
        // Чтение данных из каждой колонки
        QueryResult.append(new QStandardItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))); // Колонка 1
        QueryResult.append(new QStandardItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)))); // Колонка 2
        QueryResult.append(new QStandardItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)))); // Колонка 3
        KnownModel->appendRow(QueryResult);
    }
    sqlite3_finalize(stmt); // Завершение запроса   по итоговым файлам
    const char * UnknownQuery = "SELECT * FROM UNKNOWN_FILES;";
    if (sqlite3_prepare_v2(DB, UnknownQuery, -1, &stmt, nullptr) != SQLITE_OK) {
        wcout << L"Ошибка подготовки запроса для таблицы с известными файлами:" << sqlite3_errmsg(DB);
        return;
    }
    QStandardItemModel *UnknownModel = new QStandardItemModel();
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QList<QStandardItem*> QueryResult;
        // Чтение данных из каждой колонки
        QueryResult.append(new QStandardItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))); // Колонка 1
        QueryResult.append(new QStandardItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)))); // Колонка 2
        QueryResult.append(new QStandardItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)))); // Колонка 3
        UnknownModel->appendRow(QueryResult);
    }

    KnownModel->setColumnCount(3);
    UnknownModel->setColumnCount(3);
    KnownModel->setHorizontalHeaderLabels({"Имя","Путь","Хэш"});
    UnknownModel->setHorizontalHeaderLabels({"Имя","Путь","Хэш"});
    sqlite3_finalize(stmt);
    emit ModelsReady(KnownModel, UnknownModel);
}

void Slave::doWork()
{
    emit WorkStart();
    QString DBPATH = ReadFiles(NsrlFile, ScanDir);
    GetDataFromDB(DBPATH);
    emit finished();
}
