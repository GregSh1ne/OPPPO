#ifndef SYMPHONY_H
#define SYMPHONY_H

#include "MusicalWork.h"

class Symphony : public MusicalWork {
private:
    std::string composer;

public:
    Symphony(std::string trackTitle, double trackDuration, std::string composerName);

    [[nodiscard]] auto getType() const -> std::string override;
    auto print(std::ostream& outputStream) const -> void override;
    [[nodiscard]] auto matches(const std::string& fieldName, const std::string& operation, const std::string& filterValue) const -> bool override;
};

#endif // SYMPHONY_H