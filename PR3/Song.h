#ifndef SONG_H
#define SONG_H

#include "MusicalWork.h"

class Song : public MusicalWork {
private:
    std::string artist;

public:
    Song(std::string trackTitle, double trackDuration, std::string artistName);

    [[nodiscard]] auto getType() const -> std::string override;
    auto print(std::ostream& outputStream) const -> void override;
    [[nodiscard]] auto matches(const std::string& fieldName, const std::string& operation, const std::string& filterValue) const -> bool override;
};

#endif // SONG_H