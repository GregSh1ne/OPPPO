#include "DurationUtils.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace DurationUtils {

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

} // namespace DurationUtils