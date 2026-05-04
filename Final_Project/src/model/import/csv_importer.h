/**
 * @file model/import/csv_importer.h
 * @brief CSV file importer for saved event records (C++17)
 *
 * MVC Role: MODEL — data import from CSV files.
 */

#pragma once

#include <string>
#include <vector>
#include "model/types/event_record.h"

namespace EventViewer {

class CsvImporter {
public:
    std::vector<EventRecord> load(const std::string& path) const;

private:
    static std::vector<std::string> parseCsvRow(const std::string& line);
};

} // namespace EventViewer
