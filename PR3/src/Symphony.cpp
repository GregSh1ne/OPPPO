#include "Symphony.h"
#include <iomanip>

Symphony::Symphony(std::string trackTitle, double trackDuration, std::string composerName)
    : MusicalWork(std::move(trackTitle), trackDuration), composer(std::move(composerName)) {}

auto Symphony::getType() const -> std::string {
    return "Symphony";
}

auto Symphony::print(std::ostream& outputStream) const -> void {
    outputStream << std::left << std::setw(Config::COLUMN_TYPE_WIDTH) << "[Симфония]"
                 << " | Название: " << std::setw(Config::COLUMN_TITLE_WIDTH) << ("\"" + title + "\"")
                 << " | Длительность: " << std::setw(Config::COLUMN_TIME_WIDTH) << DurationUtils::formatDuration(duration)
                 << " (" << std::fixed << std::setprecision(2) << duration << " мин.)"
                 << " | Композитор:  " << composer;
}

auto Symphony::matches(const std::string& fieldName, const std::string& operation, const std::string& filterValue) const -> bool {
    if (MusicalWork::matches(fieldName, operation, filterValue)) {
        return true;
    }

    const std::string loweredField = DurationUtils::toLower(fieldName);
    if (loweredField == "composer") {
        const std::string loweredComposer = DurationUtils::toLower(composer);
        const std::string loweredTarget = DurationUtils::toLower(filterValue);
        if (operation == "==") { return loweredComposer == loweredTarget; }
        if (operation == "!=") { return loweredComposer != loweredTarget; }
    }
    return false;
}