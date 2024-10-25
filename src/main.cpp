#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include "nsrlRepository/nsrlRepository.h"
#include "getFileFromDir/getFileFromDir.h"
#include "OutputDB/outputDB.h"
#include "calculateShaHash/calculateShaHash.h"
#include "../models/fileSchema.h"

/** 
 * @file main.cpp
 * Основной ход программы
 */
int main(int argc, char const *argv[])
{
    vector<FilePtr> filename = getFileFromDir("../tests"); // Рекурсивный обход указанной директории
    NSRLRepository nsrlRepo = NSRLRepository("../src/nsrlRepository/test.db"); // Инициализация NSRL репозитория
    OutputDB ourDatabase; // Создание выходных баз данных

    for (int i = 0; i < filename.size(); i++)
    {
        CalculateSHA1Hash(filename[i]); // Подсчет хеша
        nsrlRepo.IsHashInDB(filename[i]); // Анализ контрольной суммы файла 
        ourDatabase.FillTheDB(filename[i]); // Заполнение баз данных
    }
    filename.clear();
}