#include "WorkCatalog.h"
#include "Song.h"
#include "Symphony.h"
#include "DurationUtils.h"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

auto WorkCatalog::add(std::unique_ptr<MusicalWork> work) -> void {
    works.push_back(std::move(work));
}

auto WorkCatalog::removeMatching(const std::string& fieldName, const std::string& operation, const std::string& filterValue) -> size_t {
    const size_t initialSize = works.size();
    works.erase(
        std::remove_if(works.begin(), works.end(), [&](const std::unique_ptr<MusicalWork>& item) -> bool {
            return item->matches(fieldName, operation, filterValue);
        }),
        works.end()
    );
    return initialSize - works.size();
}

auto WorkCatalog::print(std::ostream& outputStream) const -> void {
    outputStream << "\n================ ТЕКУЩЕЕ СОДЕРЖИМОЕ КАТАЛОГА ================\n";
    if (works.empty()) {
        outputStream << "  (Каталог пуст)\n";
    } else {
        for (size_t index = 0; index < works.size(); ++index) {
            outputStream << std::right << std::setw(2) << (index + 1) << ". ";
            works[index]->print(outputStream);
            outputStream << "\n";
        }
    }
    outputStream << "Всего объектов в каталоге: " << works.size() << "\n";
    outputStream << "============================================================\n\n";
}

auto WorkCatalog::processCommandFile(const std::string& filepath) -> void {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Ошибка: не удалось открыть файл " << filepath << "\n";
        return;
    }

    std::string currentLine;
    int lineNumber = 0;

    while (std::getline(file, currentLine)) {
        lineNumber++;
        if (currentLine.empty() || currentLine[0] == '#') {
            continue;
        }

        std::istringstream lineStream(currentLine);
        std::string commandName;
        lineStream >> commandName;

        if (commandName == "ADD") {
            std::string kind;
            std::string trackTitle;
            std::string durationStr;
            std::string personName;

            lineStream >> kind >> std::quoted(trackTitle) >> durationStr >> std::quoted(personName);

            double durationMinutes = 0.0;
            if (!DurationUtils::parseDuration(durationStr, durationMinutes)) {
                std::cerr << "Строка " << lineNumber << ": ошибка формата длительности '" 
                          << durationStr << "' (секунды должны быть от 0 до 59)\n";
                continue;
            }

            const std::string kindLower = DurationUtils::toLower(kind);
            if (kindLower == "song") {
                add(std::make_unique<Song>(trackTitle, durationMinutes, personName));
                std::cout << "[ADD] Добавлена песня: \"" << trackTitle << "\" (" << DurationUtils::formatDuration(durationMinutes) << ")\n";
            } else if (kindLower == "symphony") {
                add(std::make_unique<Symphony>(trackTitle, durationMinutes, personName));
                std::cout << "[ADD] Добавлена симфония: \"" << trackTitle << "\" (" << DurationUtils::formatDuration(durationMinutes) << ")\n";
            } else {
                std::cerr << "Строка " << lineNumber << ": неизвестный тип: " << kind << "\n";
            }
        } else if (commandName == "REM") {
            std::string fieldName;
            std::string operation;
            std::string filterValue;

            lineStream >> fieldName >> operation >> std::quoted(filterValue);

            const size_t removedCount = removeMatching(fieldName, operation, filterValue);
            std::cout << "[REM] Удалено по условию (" << fieldName << " " << operation << " \"" << filterValue << "\"): " 
                      << removedCount << " шт.\n";
        } else if (commandName == "PRINT") {
            print(std::cout);
        } else {
            std::cerr << "Строка " << lineNumber << ": неизвестная команда: " << commandName << "\n";
        }
    }
}