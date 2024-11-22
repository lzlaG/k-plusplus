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
#include <future>
#include <QDateTime>

QString Slave::ReadFiles(QString NsrlFile, QString ScanDir)
{

    QString PathToDB = QCoreApplication::applicationDirPath()+
            QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss")+".db"; //уникальное имя для бд
    std::filesystem::path ScanDirCorrect = ScanDir.toStdString();
    filesystem::path CorrectPath = ScanDirCorrect;
    vector<FilePtr> filename = getFileFromDir(CorrectPath);               // Рекурсивный обход указанной директории
    NSRLRepository nsrlRepo = NSRLRepository(NsrlFile.toStdString());    // Инициализация NSRL репозитория
    OutputDB ourDatabase = OutputDB(PathToDB.toStdString());   // Создание выходной базы данных
    emit ChangeRange(filename.size()); //издаем сигнал об изменении диапазона

    for (int i = 0; i < filename.size(); i++)
    {
        if (i%70 == 0)
        {
            emit AnekdotTime();
        };
        future<void> a1 = async([filename, i]                         // Анализ контрольной суммы файла
                                { CalculateSHA1Hash(filename[i]); }); // Подсчет хеша
        a1.wait();
        future<void> a2 = async([&nsrlRepo, filename, i]
                                { nsrlRepo.IsHashInDB(filename[i]); });
        a2.wait();
        ourDatabase.FillTheDB(filename[i]); // Заполнение базы данных
        emit ProgressUpdated(i+1);
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
        qDebug() << "Ошибка подготовки запроса для таблицы с известными файлами:" << sqlite3_errmsg(DB);
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
        qDebug() << "Ошибка подготовки запроса для таблицы с известными файлами:" << sqlite3_errmsg(DB);
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
