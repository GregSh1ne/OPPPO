#ifndef WORK_CATALOG_H
#define WORK_CATALOG_H

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include "MusicalWork.h"

class WorkCatalog {
private:
    std::vector<std::unique_ptr<MusicalWork>> works;

public:
    WorkCatalog() = default;
    ~WorkCatalog() = default;

    WorkCatalog(const WorkCatalog&) = delete;
    auto operator=(const WorkCatalog&) -> WorkCatalog& = delete;
    WorkCatalog(WorkCatalog&&) noexcept = default;
    auto operator=(WorkCatalog&&) noexcept -> WorkCatalog& = default;

    auto add(std::unique_ptr<MusicalWork> work) -> void;
    auto removeMatching(const std::string& fieldName, const std::string& operation, const std::string& filterValue) -> size_t;
    auto print(std::ostream& outputStream) const -> void;
    auto processCommandFile(const std::string& filepath) -> void;
};

#endif // WORK_CATALOG_H