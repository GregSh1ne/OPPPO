#ifndef DURATION_UTILS_H
#define DURATION_UTILS_H

#include <string>
#include <string_view>

namespace Config {
    constexpr int SECONDS_PER_MINUTE = 60;
    constexpr double SECONDS_PER_MINUTE_DOUBLE = 60.0;
    constexpr double TIME_EPSILON = 1.0 / 60.0;

    constexpr int COLUMN_TYPE_WIDTH = 12;
    constexpr int COLUMN_TITLE_WIDTH = 24;
    constexpr int COLUMN_TIME_WIDTH = 6;
}

namespace DurationUtils {
    [[nodiscard]] auto toLower(std::string_view sourceText) -> std::string;
    [[nodiscard]] auto parseDuration(const std::string& durationString, double& outMinutes) -> bool;
    [[nodiscard]] auto formatDuration(double durationInMinutes) -> std::string;
}

#endif // DURATION_UTILS_H