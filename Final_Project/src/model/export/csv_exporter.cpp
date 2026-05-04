/**
 * @file model/export/csv_exporter.cpp
 * @brief CsvExporter implementation (C++17)
 *
 * MVC Role: MODEL — writes event data to CSV format.
 */

#include "csv_exporter.h"
#include <fstream>

namespace EventViewer {

bool CsvExporter::exportEvents(const std::string& path,
                                const std::vector<EventRecord>& events) const
{
    std::ofstream f(path);
    if (!f.is_open()) return false;

    f << "EventID,Level,Source,Timestamp,Message\n";

    for (const auto& e : events) {
        f << e.eventId()           << ','
          << e.levelString()       << ','
          << quoteField(e.source())    << ','
          << quoteField(e.timestamp()) << ','
          << quoteField(e.message())   << '\n';
    }

    return f.good();
}

std::string CsvExporter::quoteField(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 2);
    out += '"';
    for (char c : s) {
        if (c == '"') out += '"';
        out += c;
    }
    out += '"';
    return out;
}

} // namespace EventViewer
