#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QCompleter>
#include <QFileSystemModel>
#include <QDebug>
#include <QTreeView>
#include <QTabWidget>
#include "../../lib/sqlite3/sqlite3.h"
#include "slave.h"



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

    //делаем таблицы не редактируемыми
    KnownTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    UnknownTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // задаем модели для таблиц
    KnownTable->setModel(KnownModel);
    UnknownTable->setModel(UnknownModel);

    //задаем стартовое значение прогресс бара
    ui->progressBar->setValue(0);
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
    // подсчет файлов в директории
    QDir dir(ScanDir);
    dir.setFilter( QDir::AllEntries | QDir::NoDotAndDotDot );
    int total_files = dir.count();
    ui->progressBar->setRange(0, total_files);
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



void MainWindow::on_pushButton_clicked()
{
    if (NsrlFile.isEmpty() != true && ScanDir.isEmpty() != true )
    {
        QTreeView *unknownview = getTreeViewFromTab(ui->tabWidget, 1);
        QTreeView *knownview = getTreeViewFromTab(ui->tabWidget,0);
        Slave *slave1 =new Slave(ScanDir, NsrlFile, KnownModel, UnknownModel, knownview, unknownview);
        //инициализируем поток и перемещаем туда объект
        Thread = new QThread(this);
        slave1->moveToThread(Thread);

        //соединяем сигналы, чтобы обозначить, что будет выполняться в потоке
        QObject::connect(Thread, &QThread::started, slave1, &Slave::doWork);
        QObject::connect(slave1, &Slave::destroyed, Thread, &QThread::quit);

        // сигналы для правильного завершения потоков
        QObject::connect(slave1, &Slave::finished, Thread, &QThread::quit);
        QObject::connect(slave1, &Slave::finished, slave1, &Slave::deleteLater);
        QObject::connect(Thread, &QThread::finished, Thread, &QThread::deleteLater);

        Thread->start();
    }
}

