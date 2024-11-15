#include <iostream>
#include <string>
#include <vector>

#include "../application/cli/application.h"

using namespace std;

/**
 * @file cli_main.cpp
 * Инстализация приложения и запуск(cli версия)
 */
int main(int argc, const char *argv[])
{
    Application app(argc, argv);
    return app.exec();
}
