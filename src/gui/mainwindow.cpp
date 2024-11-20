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
#include "anekdots.h"
#include <QSortFilterProxyModel>


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

    ui->ScanDirLine->setPlaceholderText("Укажите сканируемую директорию...");
    ui->NsrlFileLine->setPlaceholderText("Укажите путь до NSRL БД...");

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

    //начальное значение прогресс бара
    ui->progressBar->setValue(0);

    //начальное значение лейбла анекдотов
    ui->AnekdotLabel->setText("");

    //блокируем поиск файлов, пока не выполнится обработка
    ui->searchButton->setDisabled(true);
    ui->searchLine->setReadOnly(true);
    ui->searchLine->setDisabled(true);
};

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

void MainWindow::updateProgress(int value) {
    ui->progressBar->setValue(value);  // Обновление значения прогресс-бара
};

void MainWindow::RangeUpdate(int value)
{
    ui->progressBar->setRange(0, value);
};

void MainWindow::AnekdotUpdate()
{
    int random_anek = rand()%anekdots.size();
    QString NewAnekdot = QString::fromStdString(anekdots[random_anek]);
    ui->AnekdotLabel->setText(NewAnekdot);
}

void MainWindow::BlockButtons()
{
    ui->SetupDirButton->setEnabled(false);
    ui->SetupNsrlButton->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->searchButton->setEnabled(false);
    ui->searchLine->setReadOnly(true);
    ui->searchLine->setDisabled(true);
}
void MainWindow::UnblockButtons()
{
    ui->SetupDirButton->setEnabled(true);
    ui->SetupNsrlButton->setEnabled(true);
    ui->pushButton->setEnabled(true);
    ui->searchButton->setEnabled(true);
    ui->searchLine->setReadOnly(false);
    ui->searchLine->setDisabled(false);
}
void MainWindow::on_pushButton_clicked()
{
    if (NsrlFile.isEmpty() != true && ScanDir.isEmpty() != true )
    {
        //обнуляем значение прогресс бара
        ui->progressBar->setValue(0);

        //очистка данных с прошлого запуска
        KnownModel->clear();
        UnknownModel->clear();

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

        //задаем обновить диапазон прогрессбара(когда испустится сигнал)
        QObject::connect(slave1, &Slave::ChangeRange, this, &MainWindow::RangeUpdate);

        //обновляем прогресс бар
        QObject::connect(slave1, &Slave::ProgressUpdated, this, &MainWindow::updateProgress);

        connect(slave1, &Slave::WorkStart, this, &MainWindow::BlockButtons); //блокировка кнопок на время выполнения потока
        connect(slave1, &Slave::finished, this, &MainWindow::UnblockButtons); //разблокировка

        //обновляем анекдот
        connect(slave1, &Slave::AnekdotTime, this, &MainWindow::AnekdotUpdate);
        Thread->start(); //начинаем обработку
    }
}

void MainWindow::on_searchButton_clicked()
{
    //запрос
    QString search_file = ui->searchLine->text();
    QRegularExpression regex(search_file, QRegularExpression::CaseInsensitiveOption);

    // Прокси-модель для поиска в таблице известных файлов
    QSortFilterProxyModel *KnownProxyModel = new QSortFilterProxyModel(this);
    KnownProxyModel->setSourceModel(KnownModel);
    KnownProxyModel->setFilterKeyColumn(-1);
    KnownProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive); // Нечувствительность к регистру
    QTreeView* KnownTable = getTreeViewFromTab(ui->tabWidget, 0);
    KnownProxyModel->setFilterRegularExpression(regex);
    KnownTable->setModel(KnownProxyModel);

    //поиск в неизвестных файлах
    QSortFilterProxyModel *UnknownProxyModel = new QSortFilterProxyModel(this);
    UnknownProxyModel->setSourceModel(UnknownModel);
    UnknownProxyModel->setFilterKeyColumn(-1);
    UnknownProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    QTreeView* UnknownTable = getTreeViewFromTab(ui->tabWidget, 1);
    UnknownProxyModel->setFilterRegularExpression(regex);
    UnknownTable->setModel(UnknownProxyModel);
    UnknownTable->setModel(UnknownProxyModel);
}

