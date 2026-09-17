#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <iomanip>
#include <algorithm>
#include <cmath>
#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// Приведение строки к нижнему регистру
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(tolower(c));
    });
    return s;
}

// Парсинг времени
bool parseDuration(const string& str, double& outMinutes) {
    auto colonPos = str.find(':');
    if (colonPos != string::npos) {
        try {
            int mins = stoi(str.substr(0, colonPos));
            int secs = stoi(str.substr(colonPos + 1));

            if (mins < 0 || secs < 0 || secs >= 60) {
                return false; // Ошибка: секунды не могут быть 60 и более
            }

            outMinutes = mins + static_cast<double>(secs) / 60.0;
            return true;
        } catch (...) {
            return false;
        }
    } else {
        try {
            outMinutes = stod(str);
            return outMinutes >= 0.0;
        } catch (...) {
            return false;
        }
    }
}

// Форматирование длительности в "М:СС"
string formatDuration(double durationInMinutes) {
    int totalSeconds = static_cast<int>(round(durationInMinutes * 60.0));
    int mins = totalSeconds / 60;
    int secs = totalSeconds % 60;

    ostringstream oss;
    oss << mins << ":" << setw(2) << setfill('0') << secs;
    return oss.str();
}

// Базовый класс: Музыкальное произведение
class MusicalWork {
protected:
    string title;
    double duration; // Дробное число

public:
    MusicalWork(string t, double d)
        : title(move(t)), duration(d) {}

    virtual ~MusicalWork() = default;

    const string& getTitle() const { return title; }
    double getDuration() const { return duration; }

    virtual string getType() const = 0;
    virtual void print(ostream& os) const = 0;

    virtual bool matches(const string& field, const string& op, const string& val) const {
        string f = toLower(field);

        if (f == "duration") {
            double target = 0.0;
            if (parseDuration(val, target)) {
                if (op == "==") return abs(duration - target) < (1.0 / 60.0); // точность до 1 секунды
                if (op == "!=") return abs(duration - target) >= (1.0 / 60.0);
                if (op == ">")  return duration > target;
                if (op == "<")  return duration < target;
                if (op == ">=") return duration >= target;
                if (op == "<=") return duration <= target;
            }
            return false;
        } else if (f == "title") {
            string tLower = toLower(title);
            string vLower = toLower(val);
            if (op == "==") return tLower == vLower;
            if (op == "!=") return tLower != vLower;
        } else if (f == "type") {
            string typeLower = toLower(getType());
            string vLower = toLower(val);
            if (op == "==") return typeLower == vLower;
            if (op == "!=") return typeLower != vLower;
        }

        return false;
    }
};

// Класс-наследник: Песня
class Song : public MusicalWork {
private:
    string artist;

public:
    Song(string t, double d, string a)
        : MusicalWork(move(t), d), artist(move(a)) {}

    string getType() const override { return "Song"; }

    void print(ostream& os) const override {
        os << left << setw(12) << "[Песня]"
           << " | Название: " << setw(24) << ("\"" + title + "\"")
           << " | Длительность: " << setw(6) << formatDuration(duration)
           << " (" << fixed << setprecision(2) << duration << " мин.)"
           << " | Исполнитель: " << artist;
    }

    bool matches(const string& field, const string& op, const string& val) const override {
        if (MusicalWork::matches(field, op, val)) return true;

        string f = toLower(field);
        if (f == "artist" || f == "performer") {
            string aLower = toLower(artist);
            string vLower = toLower(val);
            if (op == "==") return aLower == vLower;
            if (op == "!=") return aLower != vLower;
        }
        return false;
    }
};

// Класс-наследник: Симфония
class Symphony : public MusicalWork {
private:
    string composer;

public:
    Symphony(string t, double d, string c)
        : MusicalWork(move(t), d), composer(move(c)) {}

    string getType() const override { return "Symphony"; }

    void print(ostream& os) const override {
        os << left << setw(12) << "[Симфония]"
           << " | Название: " << setw(24) << ("\"" + title + "\"")
           << " | Длительность: " << setw(6) << formatDuration(duration)
           << " (" << fixed << setprecision(2) << duration << " мин.)"
           << " | Композитор:  " << composer;
    }

    bool matches(const string& field, const string& op, const string& val) const override {
        if (MusicalWork::matches(field, op, val)) return true;

        string f = toLower(field);
        if (f == "composer") {
            string cLower = toLower(composer);
            string vLower = toLower(val);
            if (op == "==") return cLower == vLower;
            if (op == "!=") return cLower != vLower;
        }
        return false;
    }
};

// Контейнер и обработчик команд
class WorkCatalog {
private:
    vector<unique_ptr<MusicalWork>> works;

public:
    void add(unique_ptr<MusicalWork> work) {
        works.push_back(move(work));
    }

    size_t removeMatching(const string& field, const string& op, const string& val) {
        size_t initialSize = works.size();
        works.erase(
            remove_if(works.begin(), works.end(), [&](const unique_ptr<MusicalWork>& item) {
                return item->matches(field, op, val);
            }),
            works.end()
        );
        return initialSize - works.size();
    }

    void print(ostream& os) const {
        os << "\n================ ТЕКУЩЕЕ СОДЕРЖИМОЕ КАТАЛОГА ================\n";
        if (works.empty()) {
            os << "  (Каталог пуст)\n";
        } else {
            for (size_t i = 0; i < works.size(); ++i) {
                os << right << setw(2) << (i + 1) << ". ";
                works[i]->print(os);
                os << "\n";
            }
        }
        os << "Всего объектов в каталоге: " << works.size() << "\n";
        os << "============================================================\n\n";
    }

    void processCommandFile(const string& filepath) {
        ifstream file(filepath);
        if (!file.is_open()) {
            cerr << "Ошибка: не удалось открыть файл " << filepath << "\n";
            return;
        }

        string line;
        int lineNum = 0;

        while (getline(file, line)) {
            lineNum++;
            if (line.empty() || line[0] == '#') continue;

            istringstream iss(line);
            string command;
            iss >> command;

            if (command == "ADD") {
                string kind, title, durationStr, person;
                iss >> kind >> quoted(title) >> durationStr >> quoted(person);

                double durationMinutes = 0.0;
                if (!parseDuration(durationStr, durationMinutes)) {
                    cerr << "Строка " << lineNum << ": некорректный формат длительности '" 
                              << durationStr << "' (секунды должны быть от 0 до 59)\n";
                    continue;
                }

                if (toLower(kind) == "song") {
                    add(make_unique<Song>(title, durationMinutes, person));
                    cout << "[ADD] Добавлена песня: \"" << title << "\" (" << formatDuration(durationMinutes) << ")\n";
                } else if (toLower(kind) == "symphony") {
                    add(make_unique<Symphony>(title, durationMinutes, person));
                    cout << "[ADD] Добавлена симфония: \"" << title << "\" (" << formatDuration(durationMinutes) << ")\n";
                } else {
                    cerr << "Строка " << lineNum << ": неизвестный тип: " << kind << "\n";
                }
            } else if (command == "REM") {
                string field, op, val;
                iss >> field >> op >> quoted(val);

                size_t removed = removeMatching(field, op, val);
                cout << "[REM] Удалено по условию (" << field << " " << op << " \"" << val << "\"): " 
                          << removed << " шт.\n";
            } else if (command == "PRINT") {
                print(cout);
            } else {
                cerr << "Строка " << lineNum << ": неизвестная команда: " << command << "\n";
            }
        }
    }
};

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif

    WorkCatalog catalog;
    const string filename = "commands.txt";

    cout << "Запуск обработки команд из файла '" << filename << "'...\n";
    catalog.processCommandFile(filename);

    return 0;
}