#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QCompleter>
#include <QFileSystemModel>
#include <QDebug>
#include "../application/application.h"
#include <QTreeView>
#include <QTabWidget>
#include "../../lib/sqlite3/sqlite3.h"



QTreeView* getTreeViewFromTab(QTabWidget* tabWidget, int tabIndex) {

    // Получаем виджет с указанной вкладки
    QWidget* tabContent = tabWidget->widget(tabIndex);
    // Ищем QTreeView внутри вкладки
    QTreeView* treeView = tabContent->findChild<QTreeView*>();
    if (!treeView) {
        qWarning() << "QTreeView not found on tab" << tabIndex;
    }
    return treeView;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //параметры строк
    ui->ScanDirLine->setReadOnly(true);
    ui->NsrlFileLine->setReadOnly(true);
    //параметры моделей
    KnownModel->setColumnCount(3);
    UnknownModel->setColumnCount(3);
    KnownModel->setHorizontalHeaderLabels({"Имя","Путь","Хэш"});
    UnknownModel->setHorizontalHeaderLabels({"Имя","Путь","Хэш"});

    //достаем таблицы из вкладок
    QTreeView* KnownTable = getTreeViewFromTab(ui->tabWidget, 0);
    QTreeView* UnknownTable = getTreeViewFromTab(ui->tabWidget, 1);

    // задаем модели для таблиц
    KnownTable->setModel(KnownModel);
    UnknownTable->setModel(UnknownModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_SetupDirButton_clicked()
{
    ScanDir = QFileDialog::getExistingDirectory(this, tr("Выберите директорию для сканирования"),
                                                "",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    if (ScanDir.isEmpty())
            return;
    ui->ScanDirLine->setText(ScanDir);
}


void MainWindow::on_SetupNsrlButton_clicked()
{
    NsrlFile = QFileDialog::getOpenFileName(this,
                                                    tr("Выберите NSRL БД"),
                                                    "",
                                                    tr("Базы данных (*.db)"));
    if (NsrlFile.isEmpty())
            return;
    ui->NsrlFileLine->setText(NsrlFile);
}

QString MainWindow::ReadFiles(QString NsrlFile, QString ScanDir)
{
    // читаем параметры
    QString nsrlFile = NsrlFile;
    QString scanDir = ScanDir;
    QString outputDbPath = QCoreApplication::applicationDirPath()+"SMESHARIK.db";
    //QString outputDbName = "SHAMAN_LEATHER_PANTS.db";

    // Создаем хранилище строк
    std::vector<std::string> argStorage = {
        "program_name",
        "--nsrl-db-path", nsrlFile.toStdString(),
        "--scan-dir", scanDir.toStdString(),
        "--output-db-path", outputDbPath.toStdString()
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

    return outputDbPath;
}

void MainWindow::FillTreeView(QTreeView* treeView, QStandardItemModel *neededModel, const char* queryStr, QString DB_path)
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

void MainWindow::on_pushButton_clicked()
{
    if (NsrlFile.isEmpty() != true && ScanDir.isEmpty() != true )
    {
        QString DB_path = ReadFiles(NsrlFile, ScanDir);
        QTreeView *unknownview = getTreeViewFromTab(ui->tabWidget, 1);
        QTreeView *knownview = getTreeViewFromTab(ui->tabWidget,0);
        FillTreeView(unknownview, UnknownModel,"SELECT * FROM UNKNOWN_FILES;", DB_path);
        FillTreeView(knownview, KnownModel, "SELECT * FROM KNOWN_FILES;", DB_path);
    }
}

