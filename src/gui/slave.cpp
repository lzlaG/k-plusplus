#include "slave.h"
#include <QString>
#include <QStandardItemModel>
#include <QCoreApplication>
#include "../application/gui/application.h"
#include "../../lib/sqlite3/sqlite3.h"
#include <filesystem>

QString Slave::ReadFiles(QString NsrlFile, QString ScanDir)
{
    QString PathToDB = QCoreApplication::applicationDirPath()+"SMESHARIKI.db";
    std::filesystem::path ScanDirCorrect = ScanDir.toStdString();
    // Создаем хранилище строк
    std::vector<std::string> argStorage = {
        "program_name",
        "--nsrl-db-path", NsrlFile.toStdString(),
        "--scan-dir", ScanDirCorrect,
        "--output-db-path", PathToDB.toStdString()
    };

    // Преобразуем в массив указателей
    std::vector<const char*> argv;
    for (const auto& arg : argStorage) {
        argv.push_back(arg.c_str());
    }

    // Передаем аргументы
    int argc = argv.size();
    Application app(argc, argv.data());
    app.exec();

    return PathToDB;
}

void Slave::FillTreeView(QTreeView* treeView, QStandardItemModel *neededModel, const char* queryStr, QString DB_path)
{
    sqlite3 *DB;
    sqlite3_open(DB_path.toUtf8().constData(), &DB);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(DB, queryStr, -1, &stmt, nullptr) != SQLITE_OK) {
        qDebug() << "Ошибка подготовки запроса:" << sqlite3_errmsg(DB);
        return;
    }

    // Итерация по строкам результата
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QList<QStandardItem*> QueryResult;

        // Чтение данных из каждой колонки
        QueryResult.append(new QStandardItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)))); // Колонка 1
        QueryResult.append(new QStandardItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)))); // Колонка 2
        QueryResult.append(new QStandardItem(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)))); // Колонка 3

        neededModel->appendRow(QueryResult);
    }
    treeView->setModel(neededModel);
    sqlite3_finalize(stmt); // Завершение запроса
}


void Slave::doWork()
{
    QString OMEGAPATH = ReadFiles(NsrlFile, ScanDir);
    FillTreeView(KnownView, KnownModel, "SELECT * FROM KNOWN_FILES;", OMEGAPATH);
    FillTreeView(UnknownView, UnknownModel, "SELECT * FROM UNKNOWN_FILES;", OMEGAPATH);
    emit finished();
}
