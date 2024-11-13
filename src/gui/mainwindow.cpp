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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->ScanDirLine->setReadOnly(true);
    ui->NsrlFileLine->setReadOnly(true);
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

void MainWindow::on_pushButton_clicked()
{
    if (NsrlFile.isEmpty() != true && ScanDir.isEmpty() != true )
    {
        // читаем параметры
        QString nsrlFile = NsrlFile;
        QString scanDir = ScanDir;
        QString outputDbPath = QCoreApplication::applicationDirPath();
        QString outputDbName = "SHAMAN_LEATHER_PANTS.db";

        // Создаем хранилище строк
        std::vector<std::string> argStorage = {
            "program_name",
            "--nsrl-db-path", nsrlFile.toStdString(),
            "--scan-dir", scanDir.toStdString(),
            "--output-db-path", outputDbPath.toStdString(),
            "--output-db-name", outputDbName.toStdString()
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

        model = new QStandardItemModel(this);
        QTreeView* firstTreeView = getTreeViewFromTab(ui->tabWidget, 0);
        QTreeView* secondTreeView = getTreeViewFromTab(ui->tabWidget, 1);

        model->setColumnCount(5);
        firstTreeView->setModel(model);
    }
}

