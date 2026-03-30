/**
 * @file data/export/csv_exporter.h
 * @brief Event exporter interface and CSV implementation (C++17)
 *
 * SOLID changes vs C version:
 *  - OCP : IEventExporter is the extension point. Adding JSON/XML means
 *          implementing this interface — CsvExporter never changes.
 *  - SRP : CsvExporter only writes CSV. No Windows API, no GTK.
 *  - DIP : action_handlers.cpp depends on IEventExporter*, not CsvExporter*.
 *  - ISP : Export interface is separate from import interface.
 *
 * C improvement:
 *  - CsvExporter_Export(filename, events, count) with raw pointer + int count
 *    replaced by exportEvents(path, vector<EventRecord>) — safer, no count arg.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "core/types/event_record.h"

namespace EventViewer {

// ── Interface: any exporter implements this ───────────────────────────────────
class IEventExporter {
public:
    virtual ~IEventExporter() = default;

    /** @brief Writes events to the given file path. @return true on success. */
    virtual bool exportEvents(const std::string& path,
                              const std::vector<EventRecord>& events) const = 0;

    virtual std::string formatName()    const = 0;   // e.g. "CSV"
    virtual std::string fileExtension() const = 0;   // e.g. "csv"
};

// ── Concrete: CSV implementation ──────────────────────────────────────────────
class CsvExporter : public IEventExporter {
public:
    bool        exportEvents(const std::string& path,
                             const std::vector<EventRecord>& events) const override;
    std::string formatName()    const override { return "CSV"; }
    std::string fileExtension() const override { return "csv"; }

private:
    static std::string quoteField(const std::string& s);
};

} // namespace EventViewer
