/**
 * @file gui/main.cpp
 * Инстализация приложения и запуск(gui версии)
 */
#include "mainwindow.h"

#include <QApplication>

/**
 * Инстализация приложения и запуск
 * @param [in] argc количество параметров ком. строки
 * @param [in] argv параметры ком. строки
 */
int main(int argc, char *argv[])
{
    srand(time(0));
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
