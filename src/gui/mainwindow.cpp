/**
 * @file gui/mainwindow.cpp
 * Инстализация giu версии приложения
 */
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
#include <QMessageBox>
#include "stopdialog.h"
#include <QTimer>

/**
 * Создание виджета дерева(таблички) для вывода данных
 * @param [in] tabWidget виджет, в который нужно вставить виджет дерева
 * @param [in] tabIndex порядковый номер виджета
 * @param [out] treeView указатель на созданный виджет таблицы
 */
QTreeView *getTreeViewFromTab(QTabWidget *tabWidget, int tabIndex)
{

    // Получаем виджет с указанной вкладки
    QWidget *tabContent = tabWidget->widget(tabIndex);
    // Ищем QTreeView внутри вкладки
    QTreeView *treeView = tabContent->findChild<QTreeView *>();
    if (!treeView)
    {
        qWarning() << "QTreeView not found on tab" << tabIndex;
    }
    return treeView;
}

/**
 * Функция для восстановление сканирования после остановки
 */
void MainWindow::ContinueAfterResume()
{
    slave1->resume(); // вызываем паузу в потоке
    IsPaused = false; // меняем флаг паузы
    // возращаем кнопкни в состояние до остановки
    ui->pushButton->setDisabled(true);
    ui->stopButton->setEnabled(true);
    ui->searchLine->setReadOnly(true);
    ui->searchLine->clear();
    ui->searchLine->setEnabled(false);
    ui->searchButton->setEnabled(false);
}

/**
 * Функция для остановки сканирования
 */
void MainWindow::StartFromZeroAfterPause()
{
    IsPaused = false;
    // останавливаем поток
    Thread->quit();
    Thread->terminate();
    // очищаем старые параметры сканирования
    NsrlFile.clear();
    ScanDir.clear();
    // достаем таблицы из вкладок
    QTreeView *KnownTable = getTreeViewFromTab(ui->tabWidget, 0);
    QTreeView *UnknownTable = getTreeViewFromTab(ui->tabWidget, 1);
    // очищаем таблицы
    KnownTable->setModel(nullptr);
    UnknownTable->setModel(nullptr);
    // зануляем все старые параметры и переактивируем кнопки
    ui->ScanDirLine->clear();
    ui->NsrlFileLine->clear();
    ui->searchLine->setReadOnly(true);
    ui->searchLine->setEnabled(false);
    ui->searchButton->setEnabled(false);
    ui->stopButton->setEnabled(false);
    ui->pushButton->setEnabled(true);
    ui->SetupDirButton->setEnabled(true);
    ui->SetupNsrlButton->setEnabled(true);
    ui->AnekdotLabel->setText("");
    ui->progressBar->setValue(0);
}

/**
 * Инстализация giu версии приложения (Конструктор)
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("k-pp gui");

    // параметры строк
    ui->ScanDirLine->setReadOnly(true);
    ui->NsrlFileLine->setReadOnly(true);

    ui->ScanDirLine->setPlaceholderText("Укажите сканируемую директорию...");
    ui->NsrlFileLine->setPlaceholderText("Укажите путь до NSRL БД...");
    ui->searchLine->setPlaceholderText("Введите имя, путь или хэш файла...");

    // достаем таблицы из вкладок
    QTreeView *KnownTable = getTreeViewFromTab(ui->tabWidget, 0);
    QTreeView *UnknownTable = getTreeViewFromTab(ui->tabWidget, 1);

    // делаем таблицы не редактируемыми
    KnownTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    UnknownTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // начальное значение прогресс бара
    ui->progressBar->setValue(0);

    // начальное значение лейбла анекдотов
    ui->AnekdotLabel->setText("");

    // блокируем поиск файлов, пока не выполнится обработка
    ui->searchButton->setDisabled(true);
    ui->searchLine->setReadOnly(true);
    ui->searchLine->setDisabled(true);
    ui->stopButton->setDisabled(true);
};

/**
 * Деконструктор giu версии приложения
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * Функция для обработки нажатия кнопки выбора директории для сканирования
 */
void MainWindow::on_SetupDirButton_clicked()
{
    ScanDir = QFileDialog::getExistingDirectory(this, tr("Выберите директорию для сканирования"),
                                                "",
                                                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (ScanDir.isEmpty())
        return;
    ui->ScanDirLine->setText(ScanDir);
}

/**
 * Функция для обработки нажатия кнопки выбора базы NSRL
 */
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

/**
 * Функция для обновление значения прогресс-бара
 */
void MainWindow::updateProgress(int value)
{
    ui->progressBar->setValue(ui->progressBar->value() + value); // Обновление значения прогресс-бара
};

/**
 * Функция для обновления размера прогресс бара
 */
void MainWindow::RangeUpdate(int value)
{
    ui->progressBar->setRange(0, value);
};

/**
 * Функция для блокировки кнопок, на время выполнения сканирования
 */
void MainWindow::BlockButtons()
{
    // кнопки управления потока, на время выполнения должны быть разблокированы
    ui->pushButton->setEnabled(false);
    ui->stopButton->setEnabled(true);

    // блокировка остальных кнопок
    ui->SetupNsrlButton->setEnabled(false);
    ui->searchButton->setEnabled(false);
    ui->SetupDirButton->setEnabled(false);
    ui->searchLine->setReadOnly(true);
    ui->searchLine->setDisabled(true);
}

/**
 * Функция для разблокировки кнопок
 */
void MainWindow::UnblockButtons()
{
    ui->SetupDirButton->setEnabled(true);
    ui->SetupNsrlButton->setEnabled(true);
    ui->pushButton->setEnabled(true);
    ui->searchButton->setEnabled(true);
    ui->searchLine->setReadOnly(false);
    ui->searchLine->setDisabled(false);
    ui->stopButton->setDisabled(true); // кнопку стоп блокируем, если поток завершился
}

/**
 * Функция установки моделей в QTreeView
 */
void MainWindow::handleModels(QStandardItemModel *model1, QStandardItemModel *model2)
{
    QTreeView *unknownview = getTreeViewFromTab(ui->tabWidget, 1);
    QTreeView *knownview = getTreeViewFromTab(ui->tabWidget, 0);
    model1->setParent(nullptr);
    model2->setParent(nullptr);
    originalKnownModel = model1;
    originalUnknownModel = model2;
    knownview->setModel(model1);
    unknownview->setModel(model2);
}

/**
 * Функция реализации кнопки "Старт"
 */
void MainWindow::on_pushButton_clicked()
{
    if (IsPaused)
    {
        StopDialog stopdialog;
        stopdialog.setModal(true);
        QObject::connect(&stopdialog, &StopDialog::WantJustContinue, this, &MainWindow::ContinueAfterResume);
        QObject::connect(&stopdialog, &StopDialog::WantStartFromZero, this, &MainWindow::StartFromZeroAfterPause);
        stopdialog.exec();
        return;
    }
    if (NsrlFile.isEmpty() != true && ScanDir.isEmpty() != true)
    {
        ui->ScanDirLine->setStyleSheet("QLineEdit {background-color: white;}");
        ui->NsrlFileLine->setStyleSheet("QLineEdit {background-color: white;}");
        // обнуляем значение прогресс бара
        ui->progressBar->setValue(0);

        slave1 = new Slave(ScanDir, NsrlFile);
        // инициализируем поток и перемещаем туда объект
        Thread = new QThread(this);
        slave1->moveToThread(Thread);

        // соединяем сигналы, чтобы обозначить, что будет выполняться в потоке
        QObject::connect(Thread, &QThread::started, slave1, &Slave::doWork);
        QObject::connect(slave1, &Slave::destroyed, Thread, &QThread::quit);

        // сигналы для правильного завершения потоков
        QObject::connect(slave1, &Slave::finished, Thread, &QThread::quit);
        QObject::connect(slave1, &Slave::finished, slave1, &Slave::deleteLater);
        QObject::connect(Thread, &QThread::finished, slave1, &Slave::deleteLater);

        // передаем модель
        QObject::connect(slave1, &Slave::ModelsReady, this, &MainWindow::handleModels);

        // задаем обновить диапазон прогрессбара(когда испустится сигнал)
        QObject::connect(slave1, &Slave::ChangeRange, this, &MainWindow::RangeUpdate);

        // обновляем прогресс бар
        QObject::connect(slave1, &Slave::ProgressUpdated, this, &MainWindow::updateProgress);

        connect(slave1, &Slave::WorkStart, this, &MainWindow::BlockButtons);  // блокировка кнопок на время выполнения потока
        connect(slave1, &Slave::finished, this, &MainWindow::UnblockButtons); // разблокировка

        Thread->start(); // начинаем обработку
    }
    else
    {
        if (NsrlFile.isEmpty() == true)
        {
            ui->NsrlFileLine->setStyleSheet("QLineEdit {background-color: red;}");
        }
        if (ScanDir.isEmpty() == true)
        {
            ui->ScanDirLine->setStyleSheet("QLineEdit {background-color: red;}");
        }
        QMessageBox msgBox;
        msgBox.setText("Ошибка! Проверьте, что директория для сканирования и путь до NSRL БД заданы корректно");
        msgBox.exec();
    }
}

/**
 * Функция обработка нажатие на кнопку поиска
 */
void MainWindow::on_searchButton_clicked()
{
    // таблицы
    QTreeView *KnownTable = getTreeViewFromTab(ui->tabWidget, 0);
    QTreeView *UnknownTable = getTreeViewFromTab(ui->tabWidget, 1);

    // запрос
    QString search_file = ui->searchLine->text();
    QRegularExpression regex(search_file, QRegularExpression::CaseInsensitiveOption);

    // Прокси-модель для поиска в таблице известных файлов
    QSortFilterProxyModel *KnownProxyModel = new QSortFilterProxyModel(this);
    KnownProxyModel->setSourceModel(originalKnownModel);
    KnownProxyModel->setFilterKeyColumn(-1);
    KnownProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive); // Нечувствительность к регистру
    KnownProxyModel->setFilterRegularExpression(regex);
    KnownTable->setModel(KnownProxyModel);

    // поиск в неизвестных файлах
    QSortFilterProxyModel *UnknownProxyModel = new QSortFilterProxyModel(this);
    UnknownProxyModel->setSourceModel(originalUnknownModel);
    UnknownProxyModel->setFilterKeyColumn(-1);
    UnknownProxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    UnknownProxyModel->setFilterRegularExpression(regex);
    UnknownTable->setModel(UnknownProxyModel);
}

/**
 * Функция для обновления анекдота
 */
void MainWindow::on_progressBar_valueChanged(int value)
{
    if (value % 125 == 0)
    {
        int random_anek = rand() % anekdots.size(); // Получаем случайный индекс
        QString NewAnekdot = QString::fromStdString(anekdots[random_anek]);
        ui->AnekdotLabel->setText(NewAnekdot); // Устанавливаем новый текст
    }
}

/**
 * Функция для обработки нажатия на кнопку стоп
 */
void MainWindow::on_stopButton_clicked()
{
    slave1->requestPause();
    IsPaused = true;
    ui->stopButton->setEnabled(false);
    ui->pushButton->setEnabled(true);
    // разблокировка поиска, если пользователь остановил обработку
    ui->searchLine->setReadOnly(false);
    ui->searchLine->setEnabled(true);
    ui->searchButton->setEnabled(true);
}
