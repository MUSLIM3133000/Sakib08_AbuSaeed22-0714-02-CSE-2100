# SOLID Principles — UML Diagram
## Windows Event Viewer (Refactored)

---

## 1. Single Responsibility Principle (SRP)
> *Each module has one focused reason to change.*

```
┌─────────────────────┐   ┌─────────────────────┐   ┌─────────────────────┐
│    «module»         │   │    «module»         │   │    «module»         │
│    main.c           │   │   event_record.c    │   │  event_statistics.c │
│─────────────────────│   │─────────────────────│   │─────────────────────│
│ + main()            │   │ + EventRecord_Free()│   │ + compute_stats()   │
│─────────────────────│   │─────────────────────│   │─────────────────────│
│ Responsibility:     │   │ Responsibility:     │   │ Responsibility:     │
│ App entry point     │   │ Data type lifecycle │   │ Event analysis      │
└─────────────────────┘   └─────────────────────┘   └─────────────────────┘

┌─────────────────────┐   ┌─────────────────────┐   ┌─────────────────────┐
│    «module»         │   │    «module»         │   │    «module»         │
│  time_formatter.c   │   │   string_utils.c    │   │  privilege_check.c  │
│─────────────────────│   │─────────────────────│   │─────────────────────│
│+FiletimeToString()  │   │+WcharToUtf8()       │   │+IsRunAsAdmin()      │
│─────────────────────│   │─────────────────────│   │─────────────────────│
│ Responsibility:     │   │ Responsibility:     │   │ Responsibility:     │
│ Time formatting     │   │ String conversion   │   │ Privilege detection │
└─────────────────────┘   └─────────────────────┘   └─────────────────────┘
```

---

## 2. Open/Closed Principle (OCP)
> *Open for extension (new export formats), closed for modification.*

```
         ┌──────────────────────────────┐
         │        «interface»           │
         │       ExportStrategy         │
         │──────────────────────────────│
         │ + export(records, path): int │
         └──────────────┬───────────────┘
                        │
           ┌────────────┴────────────┐
           │                         │
           ▼                         ▼
┌──────────────────────┐   ┌──────────────────────┐
│    «module»          │   │    «module»          │
│    csv_exporter.c    │   │  (future)            │
│──────────────────────│   │  json_exporter.c     │
│ + export_to_csv()    │   │──────────────────────│
│                      │   │ + export_to_json()   │
│ ← extends strategy   │   │                      │
│   without changing   │   │ ← added without      │
│   existing code      │   │   touching exporter.c│
└──────────────────────┘   └──────────────────────┘

  New formats can be added (open for extension)
  without modifying csv_exporter.c (closed for modification)
```

---

## 3. Liskov Substitution Principle (LSP)
> *Any EventLogRepository implementation is substitutable for another.*

```
         ┌──────────────────────────────────────┐
         │          «struct / interface»        │
         │          EventLogRepository          │
         │──────────────────────────────────────│
         │ name            : const char*        │
         │ open()          : void*              │
         │ read()          : int                │
         │ get_statistics(): EventStatistics    │
         │ close()         : void               │
         └──────────────────┬───────────────────┘
                            │
           ┌────────────────┴────────────────┐
           │                                 │
           ▼                                 ▼
┌─────────────────────────┐       ┌──────────────────────────┐
│  «concrete impl»        │       │  «concrete impl»         │
│  event_log_windows.c    │       │  csv_importer.c          │
│─────────────────────────│       │──────────────────────────│
│ g_windowsRepository     │       │ g_csvRepository          │
│─────────────────────────│       │──────────────────────────│
│ + EventLog_Windows_Open │       │ + CsvImporter_Open()     │
│ + EventLog_Windows_Read │       │ + CsvImporter_Read()     │
│ + EventLog_Windows_     │       │ + CsvImporter_GetStats() │
│     GetStatistics()     │       │ + CsvImporter_Close()    │
│ + EventLog_Windows_     │       │                          │
│     Close()             │       │                          │
└─────────────────────────┘       └──────────────────────────┘

  UI code calls only EventLogRepository* — both implementations
  can be swapped in without breaking the caller (LSP satisfied).
```

---

## 4. Interface Segregation Principle (ISP)
> *God-struct decomposed into focused, role-specific structs.*

### Before (Violation — one monolithic struct):
```
┌──────────────────────────────────────┐
│       AppContext  (god struct)       │
│──────────────────────────────────────│
│ window, header_bar, search_entry,    │
│ log_combo, level_combo, event_list,  │
│ status_bar, current_log[], filter,   │
│ list_store, selection, model ...     │
│  (all mixed together — 20+ fields)   │
└──────────────────────────────────────┘
```

### After (ISP satisfied — three focused structs):
```
┌───────────────────────┐  ┌───────────────────────┐  ┌───────────────────────┐
│  «struct»             │  │  «struct»             │  │  «struct»             │
│  MainWindow           │  │  EventModels          │  │  AppState             │
│───────────────────────│  │───────────────────────│  │───────────────────────│
│ window: GtkWidget*    │  │ list_store: GListStore │  │ current_log: wchar_t[]│
│ header_bar: GtkWidget*│  │ selection: GtkSingle  │  │ filter_level: int     │
│ search_entry: ...     │  │   SelectionModel*     │  │ hours_back: int       │
│ log_combo: ...        │  │───────────────────────│  │ repo: EventLog        │
│ level_combo: ...      │  │ Owns: GTK data model  │  │   Repository*         │
│ event_list: ...       │  │ population only       │  │───────────────────────│
│ status_bar: ...       │  └───────────────────────┘  │ Owns: runtime state   │
│───────────────────────│                              │ only                  │
│ Owns: layout widgets  │                              └───────────────────────┘
│ only                  │
└───────────────────────┘

  Each consumer depends only on the struct it actually needs.
```

---

## 5. Dependency Inversion Principle (DIP)
> *UI depends on the abstract repository, not on the Windows API.*

```
  HIGH-LEVEL (UI Layer)                ABSTRACTION                LOW-LEVEL (Platform Layer)
  ─────────────────────                ───────────────            ──────────────────────────

  ┌──────────────────────┐             ┌─────────────────────┐   ┌─────────────────────────┐
  │  action_handlers.c   │             │  «interface»        │   │  event_log_windows.c    │
  │──────────────────────│   depends   │  EventLogRepository │   │─────────────────────────│
  │ on_refresh_clicked() │────────────▶│─────────────────────│◀──│ g_windowsRepository     │
  │ on_export_clicked()  │     on      │ open()              │   │ (EvtQuery / EvtNext /   │
  │ on_log_changed()     │  abstraction│ read()              │   │  EvtRender)             │
  └──────────────────────┘             │ get_statistics()    │   └─────────────────────────┘
                                       │ close()             │
  ┌──────────────────────┐             │─────────────────────│   ┌─────────────────────────┐
  │  main_window.c       │             │ NOT on concrete     │   │  csv_importer.c         │
  │──────────────────────│────────────▶│ Windows API         │◀──│─────────────────────────│
  │ setup_ui()           │             └─────────────────────┘   │ g_csvRepository         │
  │ bind_repository()    │                                        │ (FILE* / fgets / sscanf)│
  └──────────────────────┘                                        └─────────────────────────┘

  Arrows show direction of dependency.
  UI modules ──▶ abstraction ◀── platform modules
  (Both high-level and low-level depend on the interface, not each other.)
```

---

## Full Module Dependency Overview

```
┌────────────────────────────────────────────────────────────────────────┐
│                          main.c (entry point)                          │
└───────────────────────────────┬────────────────────────────────────────┘
                                │
              ┌─────────────────┼──────────────────┐
              ▼                 ▼                  ▼
   ┌──────────────────┐  ┌──────────────┐  ┌─────────────────┐
   │   UI Layer       │  │  Data Layer  │  │   Core Layer    │
   │──────────────────│  │──────────────│  │─────────────────│
   │ main_window.c    │  │ csv_exporter │  │ event_record.c  │
   │ menu_bar.c       │  │ csv_importer │  │ event_log_      │
   │ tool_bar.c       │  └──────┬───────┘  │   windows.c     │
   │ sidebar.c        │         │          │ event_          │
   │ action_handlers.c│         │          │   statistics.c  │
   │ event_models.c   │         │          └────────┬────────┘
   └────────┬─────────┘         │                   │
            │                   │                   │
            └──────────┬────────┘                   │
                       ▼                            │
           ┌───────────────────────┐                │
           │  «interface»          │◀───────────────┘
           │  EventLogRepository   │
           └───────────────────────┘
                       ▲
            ┌──────────┴──────────┐
            ▼                     ▼
┌───────────────────┐   ┌──────────────────────┐
│  Utils Layer      │   │  Platform Impl       │
│───────────────────│   │──────────────────────│
│ time_formatter.c  │   │ event_log_windows.c  │
│ string_utils.c    │   │ csv_importer.c       │
│ privilege_check.c │   └──────────────────────┘
└───────────────────┘
```

---

## Summary Table

| Principle | Applied In | Mechanism |
|-----------|-----------|-----------|
| **SRP** | All 33 modules | Each file has one focused responsibility |
| **OCP** | `csv_exporter.c`, future exporters | `ExportStrategy` function pointer pattern |
| **LSP** | `event_log_windows.c`, `csv_importer.c` | Both satisfy `EventLogRepository` contract fully |
| **ISP** | `MainWindow`, `EventModels`, `AppState` | God-struct decomposed into role-specific structs |
| **DIP** | `action_handlers.c` → `EventLogRepository` | UI depends on interface, not on `winevt.h` |

---

*Authors: Team Name — Sakib08_AbuSaeed22-0714-02-CSE-2100 — March 2026*
