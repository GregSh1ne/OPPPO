#include "Song.h"
#include <iomanip>

Song::Song(std::string trackTitle, double trackDuration, std::string artistName)
    : MusicalWork(std::move(trackTitle), trackDuration), artist(std::move(artistName)) {}

auto Song::getType() const -> std::string {
    return "Song";
}

auto Song::print(std::ostream& outputStream) const -> void {
    outputStream << std::left << std::setw(Config::COLUMN_TYPE_WIDTH) << "[Песня]"
                 << " | Название: " << std::setw(Config::COLUMN_TITLE_WIDTH) << ("\"" + title + "\"")
                 << " | Длительность: " << std::setw(Config::COLUMN_TIME_WIDTH) << DurationUtils::formatDuration(duration)
                 << " (" << std::fixed << std::setprecision(2) << duration << " мин.)"
                 << " | Исполнитель: " << artist;
}

auto Song::matches(const std::string& fieldName, const std::string& operation, const std::string& filterValue) const -> bool {
    if (MusicalWork::matches(fieldName, operation, filterValue)) {
        return true;
    }

    const std::string loweredField = DurationUtils::toLower(fieldName);
    if (loweredField == "artist" || loweredField == "performer") {
        const std::string loweredArtist = DurationUtils::toLower(artist);
        const std::string loweredTarget = DurationUtils::toLower(filterValue);
        if (operation == "==") { return loweredArtist == loweredTarget; }
        if (operation == "!=") { return loweredArtist != loweredTarget; }
    }
    return false;
}