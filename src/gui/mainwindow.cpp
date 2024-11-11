#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QCompleter>
#include <QFileSystemModel>
#include <vector>
#include <QDebug>
#include "../application/application.h"
#include <iostream>

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


void MainWindow::on_pushButton_clicked()
{
    if (NsrlFile.isEmpty() != true && ScanDir.isEmpty() != true )
    {
        std::vector<QString> args = {
                               "--nsrl-db-path", "kngjkndfjkdjkbjgfdbjd",
                                "--scan-dir", ScanDir,
                               "--output-db-path", "./",
                               "--output-db-name", "SHAMAN_LEATHER_PANTS.db"};

        std::vector<const char*> cstrArgs;
        int i = 0;
        for (const auto& arg : args) {
            cstrArgs.push_back(arg.toUtf8().constData());
            std::cout << cstrArgs[i] << std::endl;
            i+=1;
        }
        qDebug("СТАРТ ОБРАБОТКИ");
        Application app(4, cstrArgs.data());
        int ok = app.exec();
        qDebug("КОНЕЦ ОБРАБОТКИ");
    }
}

