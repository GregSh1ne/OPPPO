#include "MusicalWork.h"
#include <cmath>

MusicalWork::MusicalWork(std::string trackTitle, double trackDuration)
    : title(std::move(trackTitle)), duration(trackDuration) {}

auto MusicalWork::getTitle() const -> const std::string& {
    return title;
}

auto MusicalWork::getDuration() const -> double {
    return duration;
}

auto MusicalWork::matchesDuration(const std::string& operation, const std::string& targetValue) const -> bool {
    double target = 0.0;
    if (!DurationUtils::parseDuration(targetValue, target)) {
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

auto MusicalWork::matchesString(std::string_view actualValue, const std::string& operation, std::string_view expectedValue) -> bool {
    const std::string actualLower = DurationUtils::toLower(actualValue);
    const std::string expectedLower = DurationUtils::toLower(expectedValue);
    if (operation == "==") { return actualLower == expectedLower; }
    if (operation == "!=") { return actualLower != expectedLower; }
    return false;
}

auto MusicalWork::matches(const std::string& fieldName, const std::string& operation, const std::string& filterValue) const -> bool {
    const std::string fieldLower = DurationUtils::toLower(fieldName);

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