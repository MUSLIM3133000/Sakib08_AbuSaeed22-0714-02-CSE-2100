/**
 * @file model/types/event_record.h
 * @brief Core domain type for the Windows Event Viewer (C++17)
 *
 * MVC Role: MODEL — pure data object representing a single event log entry.
 *
 * SOLID principles retained:
 *  - SRP : Event is a pure data object — no I/O, no Windows API.
 *  - OCP : EventLevel is an enum class; adding a level needs no existing edits.
 *  - LSP : Immutable value semantics — subclasses cannot break field contracts.
 *
 * C improvements:
 *  - char* + manual free()  →  std::string (RAII, no leaks)
 *  - #define constants      →  enum class EventLevel (type-safe)
 *  - EventRecord_Free()     →  destructor / automatic cleanup
 */

#pragma once

#include <string>
#include <cstdint>

namespace EventViewer {

// ── Replaces: #define EVENT_LEVEL_CRITICAL 1 … (strongly typed, no int collision)
enum class EventLevel : uint8_t {
    Unknown     = 0,
    Critical    = 1,
    Error       = 2,
    Warning     = 3,
    Information = 4,
    Verbose     = 5
};

/** Converts EventLevel → display string. Free function keeps enum a value type. */
const char* eventLevelToString(EventLevel level) noexcept;

/** Converts string → EventLevel (used by CSV importer). */
EventLevel eventLevelFromString(const std::string& s) noexcept;

/**
 * @brief Represents a single Windows event log entry.
 *
 * All string fields are std::string — no manual malloc/free needed.
 * Replacing: typedef struct { char *source; char *message; … } EventRecord;
 */
class EventRecord {
public:
    // ── Construction ─────────────────────────────────────────────────────
    EventRecord() = default;

    EventRecord(uint32_t    eventId,
                EventLevel  level,
                std::string source,
                std::string message,
                std::string timestamp,
                std::string computer    = {},
                std::string user        = {},
                uint32_t    taskCategory = 0,
                std::string keywords    = {})
        : m_eventId     (eventId)
        , m_level       (level)
        , m_source      (std::move(source))
        , m_message     (std::move(message))
        , m_timestamp   (std::move(timestamp))
        , m_computer    (std::move(computer))
        , m_user        (std::move(user))
        , m_taskCategory(taskCategory)
        , m_keywords    (std::move(keywords))
    {}

    // ── Accessors (read-only — replaces direct struct field access) ───────
    uint32_t           eventId()      const noexcept { return m_eventId; }
    EventLevel         level()        const noexcept { return m_level; }
    const std::string& source()       const noexcept { return m_source; }
    const std::string& message()      const noexcept { return m_message; }
    const std::string& timestamp()    const noexcept { return m_timestamp; }
    const std::string& computer()     const noexcept { return m_computer; }
    const std::string& user()         const noexcept { return m_user; }
    uint32_t           taskCategory() const noexcept { return m_taskCategory; }
    const std::string& keywords()     const noexcept { return m_keywords; }

    /** Convenience: level as display string (replaces EventRecord_GetLevelString). */
    const char* levelString() const noexcept { return eventLevelToString(m_level); }

private:
    uint32_t    m_eventId     = 0;
    EventLevel  m_level       = EventLevel::Unknown;
    std::string m_source;
    std::string m_message;
    std::string m_timestamp;
    std::string m_computer;
    std::string m_user;
    uint32_t    m_taskCategory = 0;
    std::string m_keywords;
};

} // namespace EventViewer
