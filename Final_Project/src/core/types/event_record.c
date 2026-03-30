/**
 * @file core/types/event_record.cpp
 * @brief EventRecord and EventLevel helper implementations.
 *
 * C improvement: EventRecord_Free() with manual free() calls is gone.
 * std::string destructs itself — zero manual memory management.
 */

#include "event_record.h"

namespace EventViewer {

const char* eventLevelToString(EventLevel level) noexcept {
    switch (level) {
        case EventLevel::Critical:    return "Critical";
        case EventLevel::Error:       return "Error";
        case EventLevel::Warning:     return "Warning";
        case EventLevel::Information: return "Information";
        case EventLevel::Verbose:     return "Verbose";
        default:                      return "Unknown";
    }
}

EventLevel eventLevelFromString(const std::string& s) noexcept {
    if (s == "Critical")    return EventLevel::Critical;
    if (s == "Error")       return EventLevel::Error;
    if (s == "Warning")     return EventLevel::Warning;
    if (s == "Information") return EventLevel::Information;
    if (s == "Verbose")     return EventLevel::Verbose;
    return EventLevel::Unknown;
}

} // namespace EventViewer
