/**
 * @file data/import/csv_importer.h
 * @brief CSV file importer for saved event records (C++17)
 *
 * SOLID changes vs C version:
 *  - SRP : CsvImporter only parses CSV → EventRecord. No Windows API, no GTK.
 *  - ISP : Import interface is separate from IEventExporter.
 *
 * C improvement:
 *  - sscanf with fixed char[] buffers (overflow risk)  →  std::getline + state machine
 *  - realloc on raw pointer  →  std::vector (safe, automatic)
 *  - EventRecord** output pointer  →  returned std::vector<EventRecord>
 *  - _strdup  →  std::string assignment
 */

#pragma once

#include <string>
#include <vector>
#include "core/types/event_record.h"

namespace EventViewer {

class CsvImporter {
public:
    /**
     * @brief Loads EventRecords from a CSV file.
     * @return Vector of parsed records (empty on failure — never throws).
     *
     * C equivalent: CsvImporter_Load(filename, &events) returning int count.
     * Caller no longer needs to free each record + the array.
     */
    std::vector<EventRecord> load(const std::string& path) const;

private:
    static std::vector<std::string> parseCsvRow(const std::string& line);
};

} // namespace EventViewer
