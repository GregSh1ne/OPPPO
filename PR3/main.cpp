#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Config {
    constexpr int SECONDS_PER_MINUTE = 60;
    constexpr double SECONDS_PER_MINUTE_DOUBLE = 60.0;
    constexpr double TIME_EPSILON = 1.0 / 60.0;

    constexpr int COLUMN_TYPE_WIDTH = 12;
    constexpr int COLUMN_TITLE_WIDTH = 24;
    constexpr int COLUMN_TIME_WIDTH = 6;
}

auto toLower(std::string_view sourceText) -> std::string {
    std::string loweredText(sourceText);
    std::transform(loweredText.begin(), loweredText.end(), loweredText.begin(), [](unsigned char character) -> char {
        return static_cast<char>(std::tolower(character));
    });
    return loweredText;
}

auto parseDuration(const std::string& durationString, double& outMinutes) -> bool {
    const auto colonPos = durationString.find(':');
    if (colonPos != std::string::npos) {
        try {
            const int parsedMinutes = std::stoi(durationString.substr(0, colonPos));
            const int parsedSeconds = std::stoi(durationString.substr(colonPos + 1));

            if (parsedMinutes < 0 || parsedSeconds < 0 || parsedSeconds >= Config::SECONDS_PER_MINUTE) {
                return false;
            }

            outMinutes = parsedMinutes + (static_cast<double>(parsedSeconds) / Config::SECONDS_PER_MINUTE_DOUBLE);
            return true;
        } catch (...) {
            return false;
        }
    }

    try {
        outMinutes = std::stod(durationString);
        return outMinutes >= 0.0;
    } catch (...) {
        return false;
    }
}

auto formatDuration(double durationInMinutes) -> std::string {
    const int totalSeconds = static_cast<int>(std::round(durationInMinutes * Config::SECONDS_PER_MINUTE_DOUBLE));
    const int displayMinutes = totalSeconds / Config::SECONDS_PER_MINUTE;
    const int displaySeconds = totalSeconds % Config::SECONDS_PER_MINUTE;

    std::ostringstream outputStream;
    outputStream << displayMinutes << ":" << std::setw(2) << std::setfill('0') << displaySeconds;
    return outputStream.str();
}

class MusicalWork {
protected:
    std::string title;
    double duration;

private:
    [[nodiscard]] auto matchesDuration(const std::string& operation, const std::string& targetValue) const -> bool {
        double target = 0.0;
        if (!parseDuration(targetValue, target)) {
            return false;
        }
        const double currentDuration = getDuration();
        if (operation == "==") { return std::abs(currentDuration - target) < Config::TIME_EPSILON; }
        if (operation == "!=") { return std::abs(currentDuration - target) >= Config::TIME_EPSILON; }
        if (operation == ">")  { return currentDuration > target; }
        if (operation == "<")  { return currentDuration < target; }
        if (operation == ">=") { return currentDuration >= target; }
        if (operation == "<=") { return currentDuration <= target; }
        return false;
    }

    [[nodiscard]] static auto matchesString(std::string_view actualValue, const std::string& operation, std::string_view expectedValue) -> bool {
        const std::string actualLower = toLower(actualValue);
        const std::string expectedLower = toLower(expectedValue);
        if (operation == "==") { return actualLower == expectedLower; }
        if (operation == "!=") { return actualLower != expectedLower; }
        return false;
    }

public:
    MusicalWork(std::string trackTitle, double trackDuration)
        : title(std::move(trackTitle)), duration(trackDuration) {}

    virtual ~MusicalWork() = default;

    [[nodiscard]] auto getTitle() const -> const std::string& { return title; }
    [[nodiscard]] auto getDuration() const -> double { return duration; }

    [[nodiscard]] virtual auto getType() const -> std::string = 0;
    virtual auto print(std::ostream& outputStream) const -> void = 0;

    [[nodiscard]] virtual auto matches(const std::string& fieldName, const std::string& operation, const std::string& filterValue) const -> bool {
        const std::string fieldLower = toLower(fieldName);

        if (fieldLower == "duration") {
            return matchesDuration(operation, filterValue);
        }
        if (fieldLower == "title") {
            return matchesString(getTitle(), operation, filterValue);
        }
        if (fieldLower == "type") {
            return matchesString(getType(), operation, filterValue);
        }
        return false;
    }
};

class Song : public MusicalWork {
private:
    std::string artist;

public:
    Song(std::string trackTitle, double trackDuration, std::string artistName)
        : MusicalWork(std::move(trackTitle), trackDuration), artist(std::move(artistName)) {}

    [[nodiscard]] auto getType() const -> std::string override { return "Song"; }

    auto print(std::ostream& outputStream) const -> void override {
        outputStream << std::left << std::setw(Config::COLUMN_TYPE_WIDTH) << "[Песня]"
                     << " | Название: " << std::setw(Config::COLUMN_TITLE_WIDTH) << ("\"" + title + "\"")
                     << " | Длительность: " << std::setw(Config::COLUMN_TIME_WIDTH) << formatDuration(duration)
                     << " (" << std::fixed << std::setprecision(2) << duration << " мин.)"
                     << " | Исполнитель: " << artist;
    }

    [[nodiscard]] auto matches(const std::string& fieldName, const std::string& operation, const std::string& filterValue) const -> bool override {
        if (MusicalWork::matches(fieldName, operation, filterValue)) {
            return true;
        }

        const std::string loweredField = toLower(fieldName);
        if (loweredField == "artist" || loweredField == "performer") {
            const std::string loweredArtist = toLower(artist);
            const std::string loweredTarget = toLower(filterValue);
            if (operation == "==") { return loweredArtist == loweredTarget; }
            if (operation == "!=") { return loweredArtist != loweredTarget; }
        }
        return false;
    }
};

class Symphony : public MusicalWork {
private:
    std::string composer;

public:
    Symphony(std::string trackTitle, double trackDuration, std::string composerName)
        : MusicalWork(std::move(trackTitle), trackDuration), composer(std::move(composerName)) {}

    [[nodiscard]] auto getType() const -> std::string override { return "Symphony"; }

    auto print(std::ostream& outputStream) const -> void override {
        outputStream << std::left << std::setw(Config::COLUMN_TYPE_WIDTH) << "[Симфония]"
                     << " | Название: " << std::setw(Config::COLUMN_TITLE_WIDTH) << ("\"" + title + "\"")
                     << " | Длительность: " << std::setw(Config::COLUMN_TIME_WIDTH) << formatDuration(duration)
                     << " (" << std::fixed << std::setprecision(2) << duration << " мин.)"
                     << " | Композитор:  " << composer;
    }

    [[nodiscard]] auto matches(const std::string& fieldName, const std::string& operation, const std::string& filterValue) const -> bool override {
        if (MusicalWork::matches(fieldName, operation, filterValue)) {
            return true;
        }

        const std::string loweredField = toLower(fieldName);
        if (loweredField == "composer") {
            const std::string loweredComposer = toLower(composer);
            const std::string loweredTarget = toLower(filterValue);
            if (operation == "==") { return loweredComposer == loweredTarget; }
            if (operation == "!=") { return loweredComposer != loweredTarget; }
        }
        return false;
    }
};

class WorkCatalog {
private:
    std::vector<std::unique_ptr<MusicalWork>> works;

public:
    auto add(std::unique_ptr<MusicalWork> work) -> void {
        works.push_back(std::move(work));
    }

    auto removeMatching(const std::string& fieldName, const std::string& operation, const std::string& filterValue) -> size_t {
        const size_t initialSize = works.size();
        works.erase(
            std::remove_if(works.begin(), works.end(), [&](const std::unique_ptr<MusicalWork>& item) -> bool {
                return item->matches(fieldName, operation, filterValue);
            }),
            works.end()
        );
        return initialSize - works.size();
    }

    auto print(std::ostream& outputStream) const -> void {
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

    auto processCommandFile(const std::string& filepath) -> void {
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
                if (!parseDuration(durationStr, durationMinutes)) {
                    std::cerr << "Строка " << lineNumber << ": ошибка формата длительности '" 
                              << durationStr << "' (секунды должны быть от 0 до 59)\n";
                    continue;
                }

                const std::string kindLower = toLower(kind);
                if (kindLower == "song") {
                    add(std::make_unique<Song>(trackTitle, durationMinutes, personName));
                    std::cout << "[ADD] Добавлена песня: \"" << trackTitle << "\" (" << formatDuration(durationMinutes) << ")\n";
                } else if (kindLower == "symphony") {
                    add(std::make_unique<Symphony>(trackTitle, durationMinutes, personName));
                    std::cout << "[ADD] Добавлена симфония: \"" << trackTitle << "\" (" << formatDuration(durationMinutes) << ")\n";
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
};

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