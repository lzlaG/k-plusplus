#include <iostream>
#include <string>
#include <vector>

#include "../application/application.h"

using namespace std;

/**
 * @file main.cpp
 * Инстализация приложения и запуск
 */
int main(int argc, const char *argv[])
{
    Application app(argc, argv);
    return app.exec();
}
