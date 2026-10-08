#ifndef MUSICAL_WORK_H
#define MUSICAL_WORK_H

#include <iostream>
#include <string>
#include <string_view>
#include "DurationUtils.h"

class MusicalWork {
protected:
    std::string title;
    double duration;

private:
    [[nodiscard]] auto matchesDuration(const std::string& operation, const std::string& targetValue) const -> bool;
    [[nodiscard]] static auto matchesString(std::string_view actualValue, const std::string& operation, std::string_view expectedValue) -> bool;

public:
    MusicalWork(std::string trackTitle, double trackDuration);
    virtual ~MusicalWork() = default;

    [[nodiscard]] auto getTitle() const -> const std::string&;
    [[nodiscard]] auto getDuration() const -> double;

    [[nodiscard]] virtual auto getType() const -> std::string = 0;
    virtual auto print(std::ostream& outputStream) const -> void = 0;

    [[nodiscard]] virtual auto matches(const std::string& fieldName, const std::string& operation, const std::string& filterValue) const -> bool;
};

#endif // MUSICAL_WORK_H