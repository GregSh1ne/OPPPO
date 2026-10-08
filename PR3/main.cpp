#include "WorkCatalog.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

auto main() -> int {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    WorkCatalog catalog;
    const std::string inputFilename = "commands.txt";

    std::cout << "Запуск обработки команд из файла '" << inputFilename << "'...\n";
    catalog.processCommandFile(inputFilename);

    return 0;
}