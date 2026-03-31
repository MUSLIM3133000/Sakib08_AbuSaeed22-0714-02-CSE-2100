# SOLID Principles — UML Diagram

---

## 1. Single Responsibility Principle (SRP)
> *Each class has one focused reason to change.*

```
┌──────────────────────────┐   ┌──────────────────────────┐   ┌──────────────────────────┐
│      «class»             │   │      «class»             │   │      «class»             │
│      main.cpp            │   │      EventRecord         │   │ EventStatisticsCalculator│
│──────────────────────────│   │──────────────────────────│   │──────────────────────────│
│ + main()                 │   │ - m_eventId: uint32_t    │   │ + calculate(logName,     │
│                          │   │ - m_level: EventLevel    │   │     hoursBack)           │
│──────────────────────────│   │ - m_source: string       │   │   : EventStatistics      │
│ Responsibility:          │   │ - m_message: string      │   │──────────────────────────│
│ Composition root only.   │   │ - m_timestamp: string    │   │ Responsibility:          │
│ Wires dependencies and   │   │──────────────────────────│   │ Aggregate event counts   │
│ starts GTK. No business  │   │ + eventId(): uint32_t    │   │ from a log channel.      │
│ logic.                   │   │ + level(): EventLevel    │   │ No Windows API exposure  │
└──────────────────────────┘   │ + source(): string       │   │ to callers.              │
                               │ + message(): string      │   └──────────────────────────┘
                               │ + levelString(): char*   │
                               │──────────────────────────│
                               │ Responsibility:          │
                               │ Pure data object.        │
                               │ No I/O, no Windows API.  │
                               └──────────────────────────┘

┌──────────────────────────┐   ┌──────────────────────────┐   ┌──────────────────────────┐
│      «class»             │   │      «class»             │   │      «class»             │
│    TimeFormatter         │   │     StringUtils          │   │    PrivilegeCheck        │
│   (time_formatter.cpp)   │   │   (string_utils.cpp)     │   │  (privilege_check.cpp)   │
│──────────────────────────│   │──────────────────────────│   │──────────────────────────│
│ + filetimeToString(      │   │ + wcharToUtf8(           │   │ + isRunAsAdmin(): bool   │
│     FILETIME): string    │   │     wstring): string     │   │──────────────────────────│
│──────────────────────────│   │──────────────────────────│   │ Responsibility:          │
│ Responsibility:          │   │ Responsibility:          │   │ Detect admin privilege.  │
│ FILETIME → readable      │   │ Wide string <-> UTF-8    │   │ Nothing else.            │
│ timestamp string only.   │   │ conversion only.         │   └──────────────────────────┘
└──────────────────────────┘   └──────────────────────────┘
```

---

## 2. Open/Closed Principle (OCP)
> *Open for extension (new export formats), closed for modification.*

```
              ┌───────────────────────────────────────┐
              │            «interface»                │
              │           IEventExporter              │
              │───────────────────────────────────────│
              │ + exportEvents(path: string,          │
              │     events: vector<EventRecord>)      │
              │     : bool                            │
              │ + formatName()    : string            │
              │ + fileExtension() : string            │
              └──────────────────┬────────────────────┘
                                 │  implements
              ┌──────────────────┴────────────────────┐
              │                                       │
              ▼                                       ▼
┌─────────────────────────────┐       ┌─────────────────────────────┐
│        «class»              │       │      «class» (future)       │
│       CsvExporter           │       │       JsonExporter          │
│─────────────────────────────│       │─────────────────────────────│
│ + exportEvents(...): bool   │       │ + exportEvents(...): bool   │
│ + formatName(): "CSV"       │       │ + formatName(): "JSON"      │
│ + fileExtension(): "csv"    │       │ + fileExtension(): "json"   │
│─────────────────────────────│       │─────────────────────────────│
│ - quoteField(s): string     │       │ Added without modifying     │
│                             │       │ CsvExporter or any caller.  │
└─────────────────────────────┘       └─────────────────────────────┘

  ActionHandlers calls ctx->eventExporter->exportEvents() via IEventExporter*.
  Swapping CSV → JSON = change one line in main.cpp only.
```

---

## 3. Liskov Substitution Principle (LSP)
> *Any ILogRepository implementation can be substituted without breaking callers.*

```
              ┌───────────────────────────────────────────┐
              │              «abstract class»             │
              │             ILogRepository                │
              │───────────────────────────────────────────│
              │ + read(logName: wchar_t*,                 │
              │     hoursBack: int,                       │
              │     maxEvents: int)                       │
              │     : vector<EventRecord>   (pure virtual)│
              │ + availableLogs()                         │
              │     : vector<wstring>       (pure virtual)│
              │ + ~ILogRepository()         (virtual)     │
              │───────────────────────────────────────────│
              │ [static] + create()                       │
              │     : unique_ptr<ILogRepository>          │
              │   Returns WindowsEventLogRepository on    │
              │   Windows; stub on other platforms.       │
              └──────────────────┬────────────────────────┘
                                 │  implements
              ┌──────────────────┴────────────────────┐
              │                                       │
              ▼                                       ▼
┌──────────────────────────────┐    ┌──────────────────────────────┐
│          «class»             │    │       «class» (future)       │
│  WindowsEventLogRepository   │    │      CsvLogRepository        │
│──────────────────────────────│    │──────────────────────────────│
│ + read(...):                 │    │ + read(...):                 │
│     vector<EventRecord>      │    │     vector<EventRecord>      │
│ + availableLogs():           │    │ + availableLogs():           │
│     vector<wstring>          │    │     vector<wstring>          │
│──────────────────────────────│    │──────────────────────────────│
│ Uses EvtQuery / EvtNext /    │    │ Reads from a CSV file via    │
│ EvtRender (winevt.h).        │    │ CsvImporter::load().         │
└──────────────────────────────┘    └──────────────────────────────┘

  Callers (EventModels, ActionHandlers) hold ILogRepository* only.
  Either concrete class can be substituted — same contract, no caller changes.
  LSP is upheld: read() never throws, always returns a valid (possibly empty) vector.
```

---

## 4. Interface Segregation Principle (ISP)
> *Focused interfaces — no class is forced to depend on methods it does not use.*

### Separate interfaces per role:
```
┌───────────────────────────────────┐    ┌───────────────────────────────────┐
│          «interface»              │    │           «class»                 │
│         IEventExporter            │    │          CsvImporter              │
│───────────────────────────────────│    │───────────────────────────────────│
│ + exportEvents(path, events): bool│    │ + load(path: string)             │
│ + formatName()    : string        │    │     : vector<EventRecord>        │
│ + fileExtension() : string        │    │───────────────────────────────────│
│───────────────────────────────────│    │ - parseCsvRow(line: string)      │
│ Consumers: ActionHandlers         │    │     : vector<string>             │
│ (save/export only)                │    │───────────────────────────────────│
│ Does NOT include read or import.  │    │ Consumers: ActionHandlers (import)│
└───────────────────────────────────┘    │ Does NOT include export.          │
                                         └───────────────────────────────────┘

┌───────────────────────────────────┐    ┌───────────────────────────────────┐
│          «interface»              │    │          «interface»              │
│         ILogRepository            │    │      IStatisticsCalculator        │
│───────────────────────────────────│    │───────────────────────────────────│
│ + read(...): vector<EventRecord>  │    │ + calculate(logName, hoursBack)  │
│ + availableLogs(): vector<wstring>│    │     : EventStatistics            │
│───────────────────────────────────│    │───────────────────────────────────│
│ Consumers: EventModels,           │    │ Consumers: ActionHandlers        │
│ ActionHandlers.                   │    │ Stats only — no read, no export. │
│ Read operations only.             │    └───────────────────────────────────┘
│ No stats, no export, no import.   │
└───────────────────────────────────┘

  Each interface covers one role. No consumer is forced to depend on methods
  it does not call.
```

### EventViewerContext — focused dependency carrier:
```
┌──────────────────────────────────────────────────────────────┐
│                  «struct»  EventViewerContext                │
│                   (event_viewer_context.h)                   │
│──────────────────────────────────────────────────────────────│
│ GTK widgets:                                                 │
│   app: GtkApplication*      window: GtkWidget*              │
│   notebook: GtkWidget*      treeView: GtkWidget*            │
│   treeStore: GtkTreeStore*                                   │
│   adminTreeView / adminStore: GtkWidget* / GtkListStore*    │
│   recentTreeView / recentStore: GtkWidget* / GtkListStore*  │
│   logTreeView / logStore: GtkWidget* / GtkListStore*        │
│   eventDetailsView / eventDetailsStore                       │
│──────────────────────────────────────────────────────────────│
│ Application state:                                           │
│   currentLogName: std::string   (replaces char* + strdup)   │
│──────────────────────────────────────────────────────────────│
│ Injected dependencies (interface pointers — DIP):           │
│   logRepository: ILogRepository*   ← never a concrete type  │
│   eventExporter: IEventExporter*   ← never a concrete type  │
└──────────────────────────────────────────────────────────────┘
```

---

## 5. Dependency Inversion Principle (DIP)
> *High-level modules depend on abstractions, not on concrete platform code.*

```
  HIGH-LEVEL (UI Layer)              ABSTRACTIONS               LOW-LEVEL (Infrastructure)
  ─────────────────────              ────────────               ──────────────────────────

  ┌─────────────────────┐           ┌──────────────────────┐   ┌──────────────────────────┐
  │     «class»         │  depends  │    «interface»       │   │       «class»            │
  │   ActionHandlers    │──────────▶│   ILogRepository     │◀──│ WindowsEventLogRepository│
  │─────────────────────│    on     │──────────────────────│   │──────────────────────────│
  │ + onRefresh()       │ interface │ + read()             │   │ (EvtQuery / EvtNext /    │
  │ + onOpenLog()       │           │ + availableLogs()    │   │  EvtRender — winevt.h)   │
  │ + onSaveLog()       │           └──────────────────────┘   └──────────────────────────┘
  │ + onImportView()    │
  └─────────────────────┘           ┌──────────────────────┐   ┌──────────────────────────┐
                                    │    «interface»       │   │       «class»            │
  ┌─────────────────────┐  depends  │   IEventExporter     │◀──│      CsvExporter         │
  │     «class»         │──────────▶│──────────────────────│   │──────────────────────────│
  │    EventModels      │    on     │ + exportEvents()     │   │ (std::ofstream, CSV      │
  │─────────────────────│ interface │ + formatName()       │   │  quoting — no Windows API│
  │ + populateTree()    │           │ + fileExtension()    │   │  no GTK)                 │
  │ + populateTables()  │           └──────────────────────┘   └──────────────────────────┘
  │ + populateEvent     │
  │     Details()       │           ┌──────────────────────┐   ┌──────────────────────────┐
  │ + loadLogEvents()   │  depends  │    «interface»       │   │       «class»            │
  └─────────────────────┘──────────▶│IStatisticsCalculator │◀──│EventStatisticsCalculator │
                            on      │──────────────────────│   │──────────────────────────│
  ┌─────────────────────┐ interface │ + calculate(logName, │   │ (reads event log counts  │
  │     «class»         │           │     hoursBack)       │   │  via ILogRepository      │
  │    MainWindow       │           │     : EventStatistics│   │  internally)             │
  │─────────────────────│           └──────────────────────┘   └──────────────────────────┘
  │ + activate() [static│
  └─────────────────────┘

  main.cpp is the ONLY file that names concrete types:
    auto repository = ILogRepository::create();   // → WindowsEventLogRepository
    auto exporter   = std::make_unique<CsvExporter>();
  All other files receive interface pointers through EventViewerContext.

  Dependency arrows:  UI ──▶ abstraction ◀── infrastructure
  Both sides depend on the interface; neither depends on the other.
```

---

## Full Class Dependency Overview (C++17)

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                    main.cpp  (Composition Root)                              │
│  Creates: ILogRepository::create()  →  WindowsEventLogRepository            │
│           std::make_unique<CsvExporter>()                                   │
│  Injects into EventViewerContext → passed to MainWindow::activate()         │
└────────────────────────────────┬─────────────────────────────────────────────┘
                                 │
              ┌──────────────────┼──────────────────────┐
              ▼                  ▼                       ▼
   ┌──────────────────┐  ┌───────────────────┐  ┌──────────────────────────┐
   │    UI Layer      │  │   Data Layer      │  │       Core Layer         │
   │──────────────────│  │───────────────────│  │──────────────────────────│
   │ MainWindow       │  │ CsvExporter       │  │ EventRecord              │
   │ ActionHandlers   │  │  : IEventExporter │  │ EventLevel (enum class)  │
   │ EventModels      │  │ CsvImporter       │  │ EventStatistics (struct) │
   │ MenuBar          │  └────────┬──────────┘  │ EventStatisticsCalculator│
   │ ToolBar          │           │             │  : IStatisticsCalculator │
   │ Sidebar          │           │             └──────────────┬───────────┘
   └────────┬─────────┘           │                            │
            │                    └───────────────────┐         │
            └──────────────────────────────────────┐ │         │
                                                   ▼ ▼         ▼
                              ┌─────────────────────────────────────┐
                              │           «interface»               │
                              │          ILogRepository             │
                              └──────────────────┬──────────────────┘
                                                 │  implements
                              ┌──────────────────┴──────────────────┐
                              ▼                                      ▼
               ┌──────────────────────────┐        ┌─────────────────────────┐
               │ WindowsEventLogRepository│        │      Utils Layer        │
               │  (platform/Windows only) │        │─────────────────────────│
               └──────────────────────────┘        │ StringUtils             │
                                                   │ TimeFormatter           │
                                                   │ PrivilegeCheck          │
                                                   └─────────────────────────┘
```

---

## Summary Table

| Principle | Applied In (C++17) | Mechanism |
|-----------|-------------------|-----------|
| **SRP** | All classes | Each class/file has one focused responsibility — no class mixes GTK, Windows API, and business logic |
| **OCP** | `IEventExporter`, `CsvExporter` | New export formats (JSON, XML) implement `IEventExporter` — zero changes to existing classes |
| **LSP** | `ILogRepository`, `WindowsEventLogRepository` | Any `ILogRepository` implementation satisfies the contract; `read()` never throws, always returns a valid vector |
| **ISP** | `ILogRepository`, `IEventExporter`, `IStatisticsCalculator` | Three separate focused interfaces; no consumer depends on methods it does not use |
| **DIP** | `ActionHandlers`, `EventModels`, `MainWindow` | All UI classes hold interface pointers (`ILogRepository*`, `IEventExporter*`); `main.cpp` is the sole composition root |

---

*Authors: Team Name — Sakib08_AbuSaeed22-0714-02-CSE-2100 — March 2026*