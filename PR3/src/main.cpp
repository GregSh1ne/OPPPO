#include "WorkCatalog.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

void configureConsoleEncoding() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
}

auto main(int argc, char* argv[]) -> int {
    configureConsoleEncoding();

    const std::string inputFilename = (argc > 1) ? argv[1] : "commands.txt";

    std::cout << "Запуск обработки команд из файла '" << inputFilename << "'...\n";
    WorkCatalog catalog;
    catalog.processCommandFile(inputFilename);

    return 0;
}