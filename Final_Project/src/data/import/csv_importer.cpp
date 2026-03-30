/**
 * @file data/import/csv_importer.cpp
 * @brief CsvImporter implementation (C++17)
 *
 * C improvements:
 *  - sscanf("%31[^,]", ...) with fixed buffers  →  proper RFC-4180 state machine
 *  - realloc loop on raw pointer  →  std::vector::push_back
 *  - _strdup for each field  →  std::string move semantics
 *  - Manual free(records[i]) in caller  →  vector destructs automatically
 */

#include "csv_importer.h"
#include <fstream>
#include <algorithm>

namespace EventViewer {

std::vector<EventRecord> CsvImporter::load(const std::string& path) const {
    std::ifstream f(path);
    if (!f.is_open()) return {};

    std::vector<EventRecord> records;
    std::string line;

    // Skip header row
    if (!std::getline(f, line)) return {};

    while (std::getline(f, line)) {
        if (line.empty()) continue;

        auto fields = parseCsvRow(line);
        if (fields.size() < 5) continue;

        try {
            uint32_t id    = static_cast<uint32_t>(std::stoul(fields[0]));
            EventLevel lvl = eventLevelFromString(fields[1]);

            records.emplace_back(id, lvl,
                fields[2],   // source
                fields[4],   // message
                fields[3]);  // timestamp
        } catch (...) {
            // Malformed row — skip silently
        }
    }
    return records;
}

// RFC-4180 compliant CSV row parser (handles quoted fields with embedded commas)
std::vector<std::string> CsvImporter::parseCsvRow(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"' && i + 1 < line.size() && line[i + 1] == '"') {
                field += '"'; ++i;  // escaped double-quote
            } else if (c == '"') {
                inQuotes = false;
            } else {
                field += c;
            }
        } else {
            if      (c == '"') { inQuotes = true; }
            else if (c == ',') { fields.push_back(std::move(field)); field.clear(); }
            else               { field += c; }
        }
    }
    fields.push_back(std::move(field));
    return fields;
}

} // namespace EventViewer
