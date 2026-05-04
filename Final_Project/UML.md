# MVC Architecture — UML Diagram

---

## MVC Overview

> *The application follows the **Model-View-Controller (MVC)** pattern,
> where each layer has a clear, focused responsibility.*

```
┌───────────────────────────────────────────────────────────────────────┐
│                        main.cpp (Entry Point)                         │
│  Creates Model infrastructure, wires View + Controller via context   │
└──────────────────────────────┬────────────────────────────────────────┘
                               │
          ┌────────────────────┼────────────────────┐
          ▼                    ▼                     ▼
┌──────────────────┐ ┌──────────────────┐ ┌──────────────────────────┐
│   MODEL Layer    │ │   VIEW Layer     │ │   CONTROLLER Layer       │
│──────────────────│ │──────────────────│ │──────────────────────────│
│ EventRecord      │ │ MainWindow       │ │ ActionHandlers           │
│ ILogRepository   │ │ MenuBar          │ │ EventController          │
│ WindowsEventLog  │ │ ToolBar          │ │  (populateTree,          │
│  Repository      │ │ Sidebar          │ │   populateTables,        │
│ EventStatistics  │ │ EventViewerCtx   │ │   loadLogEvents,         │
│ CsvExporter      │ │                  │ │   populateEventDetails)  │
│ CsvImporter      │ │                  │ │                          │
└──────────────────┘ └──────────────────┘ └──────────────────────────┘
```

---

## 1. MODEL Layer — Data & Business Logic

> *The Model contains all data structures, business logic, and data access.*

```
┌──────────────────────────────┐   ┌──────────────────────────────┐
│      «class»                 │   │      «struct»                │
│      EventRecord             │   │      EventStatistics         │
│   (model/types/)             │   │   (model/statistics/)        │
│──────────────────────────────│   │──────────────────────────────│
│ - m_eventId: uint32_t        │   │ + criticalCount: uint32_t    │
│ - m_level: EventLevel        │   │ + errorCount: uint32_t       │
│ - m_source: string           │   │ + warningCount: uint32_t     │
│ - m_message: string          │   │ + informationCount: uint32_t │
│ - m_timestamp: string        │   │ + auditSuccessCount: uint32_t│
│──────────────────────────────│   │ + auditFailureCount: uint32_t│
│ + eventId(): uint32_t        │   │──────────────────────────────│
│ + level(): EventLevel        │   │ + total(): uint32_t          │
│ + source(): string           │   └──────────────────────────────┘
│ + levelString(): char*       │
│──────────────────────────────│
│ Responsibility:              │
│ Pure data object.            │
│ No I/O, no Windows API.     │
└──────────────────────────────┘

              ┌───────────────────────────────────────┐
              │            «interface»                │
              │           ILogRepository              │
              │         (model/repository/)           │
              │───────────────────────────────────────│
              │ + read(logName, hoursBack, maxEvents) │
              │     : vector<EventRecord>             │
              │ + availableLogs(): vector<wstring>     │
              │ + [static] create(): unique_ptr        │
              └──────────────────┬────────────────────┘
                                 │  implements
                                 ▼
              ┌───────────────────────────────────────┐
              │            «class»                    │
              │    WindowsEventLogRepository          │
              │   (model/repository/windows/)         │
              │───────────────────────────────────────│
              │ Uses EvtQuery / EvtNext / EvtRender   │
              │ (winevt.h)                            │
              └───────────────────────────────────────┘

┌──────────────────────────────┐   ┌──────────────────────────────┐
│      «interface»             │   │      «class»                 │
│     IEventExporter           │   │     CsvImporter              │
│   (model/export/)            │   │   (model/import/)            │
│──────────────────────────────│   │──────────────────────────────│
│ + exportEvents(path, events) │   │ + load(path: string)         │
│     : bool                   │   │     : vector<EventRecord>    │
│ + formatName(): string       │   │──────────────────────────────│
│ + fileExtension(): string    │   │ Parses RFC-4180 CSV files.   │
│──────────────────────────────│   └──────────────────────────────┘
│      ▲ implements            │
│      │                       │
│ ┌────┴──────────────────┐    │
│ │    CsvExporter        │    │
│ │  formatName: "CSV"    │    │
│ └───────────────────────┘    │
└──────────────────────────────┘
```

---

## 2. VIEW Layer — GTK4 Widgets (Display Only)

> *The View creates and manages GTK widgets. It does NOT contain business logic.*

```
┌──────────────────────────────┐   ┌──────────────────────────────┐
│      «class»                 │   │      «struct»                │
│      MainWindow              │   │   EventViewerContext         │
│   (view/windows/)            │   │   (view/context/)            │
│──────────────────────────────│   │──────────────────────────────│
│ + activate(app, userData)    │   │ + app: GtkApplication*       │
│   [static]                   │   │ + window: GtkWidget*         │
│──────────────────────────────│   │ + notebook: GtkWidget*       │
│ Creates window layout:       │   │ + treeView / treeStore       │
│ - Menu bar (View)            │   │ + adminStore / logStore      │
│ - Toolbar  (View)            │   │ + eventDetailsStore          │
│ - Sidebar  (View)            │   │ + currentLogName: string     │
│ - Content area (tabs)        │   │ + logRepository: ILogRepo*   │
│ Delegates data population    │   │ + eventExporter: IExporter*  │
│ to EventController.          │   └──────────────────────────────┘
└──────────────────────────────┘

┌──────────────────┐  ┌──────────────────┐  ┌──────────────────┐
│    «class»       │  │    «class»       │  │    «class»       │
│    MenuBar       │  │    ToolBar       │  │    Sidebar       │
│ (view/components)│  │ (view/components)│  │ (view/components)│
│──────────────────│  │──────────────────│  │──────────────────│
│ + create(ctx)    │  │ + create(ctx)    │  │ + create(ctx)    │
│   : GtkWidget*   │  │   : GtkWidget*   │  │   : GtkWidget*   │
│──────────────────│  │──────────────────│  │──────────────────│
│ File/Action/View/│  │ Back/Forward/    │  │ Tree view with   │
│ Help menus.      │  │ Refresh buttons. │  │ log hierarchy.   │
│ Pure widget      │  │ Wires to         │  │ Wires selection  │
│ creation.        │  │ Controller.      │  │ to Controller.   │
└──────────────────┘  └──────────────────┘  └──────────────────┘
```

---

## 3. CONTROLLER Layer — User Action Handling

> *The Controller handles user input from the View, interacts with the Model,
> and updates the View accordingly.*

```
┌──────────────────────────────────────────────────────────────────┐
│                      «class» ActionHandlers                      │
│                    (controller/action_handlers)                   │
│──────────────────────────────────────────────────────────────────│
│ + onQuit(...)        — Exits the application                     │
│ + onAbout(...)       — Shows About dialog                        │
│ + onRefresh(...)     — Refreshes current log view                │
│ + onOpenLog(...)     — Opens file dialog → CsvImporter (Model)   │
│ + onSaveLog(...)     — Save dialog → IEventExporter (Model)      │
│ + onCreateView(...)  — Placeholder for custom views              │
│ + onImportView(...)  — Import custom view dialog                 │
│ + onTreeSelectionChanged(...)  — Delegates to EventController    │
│──────────────────────────────────────────────────────────────────│
│ Responsibility: Receive user input → call Model → update View    │
└──────────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────────┐
│                     «class» EventController                      │
│                  (controller/event_controller)                   │
│──────────────────────────────────────────────────────────────────│
│ + populateTree(ctx)              — Fills sidebar tree (View)     │
│ + populateTables(ctx, stats...)  — Fills overview tables (View)  │
│ + populateEventDetails(ctx, events)  — Fills event list (View)   │
│ + loadLogEvents(ctx, logName)    — Reads Model → updates View    │
│──────────────────────────────────────────────────────────────────│
│ Responsibility: Bridge Model data into View (GTK stores)         │
│ Renamed from EventModels to clarify MVC Controller role.         │
└──────────────────────────────────────────────────────────────────┘
```

---

## 4. MVC Data Flow

```
  USER clicks "Application" in sidebar (View)
       │
       ▼
  Sidebar → GtkTreeSelection "changed" signal
       │
       ▼
  ActionHandlers::onTreeSelectionChanged()     ← CONTROLLER
       │
       ├── Reads selected log name from GtkTreeModel
       │
       ▼
  EventController::loadLogEvents(ctx, "Application")  ← CONTROLLER
       │
       ├── Calls ctx->logRepository->read(...)         ← MODEL
       │       (ILogRepository → WindowsEventLogRepository)
       │
       ├── Receives vector<EventRecord>                ← MODEL data
       │
       ▼
  EventController::populateEventDetails(ctx, events)   ← CONTROLLER
       │
       └── Fills ctx->eventDetailsStore (GtkListStore)  ← VIEW updated
```

---

## 5. MVC Directory Structure

```
src/
├── main/                          ← Entry Point (Composition Root)
│   └── main.cpp
│
├── model/                         ← MODEL: Data + Business Logic
│   ├── types/
│   │   ├── event_record.h
│   │   └── event_record.cpp
│   ├── statistics/
│   │   ├── event_statistics.h
│   │   └── event_statistics.cpp
│   ├── repository/
│   │   ├── log_repository.h       (ILogRepository interface)
│   │   └── windows/
│   │       ├── event_log_windows.h
│   │       └── event_log_windows.cpp
│   ├── export/
│   │   ├── csv_exporter.h         (IEventExporter + CsvExporter)
│   │   └── csv_exporter.cpp
│   └── import/
│       ├── csv_importer.h
│       └── csv_importer.cpp
│
├── view/                          ← VIEW: GTK4 Widgets (Display)
│   ├── context/
│   │   └── event_viewer_context.h
│   ├── windows/
│   │   ├── main_window.h
│   │   └── main_window.cpp
│   └── components/
│       ├── menu_bar.h / .cpp
│       ├── tool_bar.h / .cpp
│       └── sidebar.h / .cpp
│
├── controller/                    ← CONTROLLER: User Actions
│   ├── action_handlers.h / .cpp
│   └── event_controller.h / .cpp
│
└── utils/                         ← Shared Utilities
    ├── conversion/
    │   ├── string_utils.h / .cpp
    ├── time/
    │   ├── time_formatter.h / .cpp
    └── platform/
        ├── privilege_check.h / .cpp
```

**Total:** 33 files across 16 directories

---

## Summary Table

| MVC Layer | Files | Responsibility |
|-----------|-------|---------------|
| **Model** | `event_record`, `event_statistics`, `log_repository`, `event_log_windows`, `csv_exporter`, `csv_importer` | Data structures, business logic, data access (Windows API), import/export |
| **View** | `main_window`, `menu_bar`, `tool_bar`, `sidebar`, `event_viewer_context` | GTK4 widget creation, layout, display — no business logic |
| **Controller** | `action_handlers`, `event_controller` | Handles user input, reads Model data, updates View (GTK stores) |
| **Utils** | `string_utils`, `time_formatter`, `privilege_check` | Shared helpers — no MVC role |
| **Entry** | `main.cpp` | Composition root — wires M, V, C together |

---

*Authors: Team Name — Sakib08_AbuSaeed22-0714-02-CSE-2100 — March 2026*