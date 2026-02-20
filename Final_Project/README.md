# Windows Event Viewer

A GTK4 Event Viewer for Windows

## What is this

This is my project for Advanced Programming Lab. It's a Windows Event Viewer made with GTK4 and C. You can view event logs from Windows like Application, System, Security etc.

## How to build

You need MSYS2 on Windows.

### Install stuff

```bash
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-gtk4
pacman -S mingw-w64-x86_64-pkg-config
pacman -S make
```

### Build it

```bash
make
```

### Run it

```bash
make run
```

Or just:

```bash
./build/bin/event_viewer.exe
```

## What it does

- Shows Windows Event Logs (Application, System, Security, Setup)
- Can save logs to CSV files
- Can open CSV files
- Shows statistics (last hour, 24 hours, 7 days)

**Note:** You need admin rights to see Security logs.

## Files

The code is organized into folders:

- `src/main/` - main program
- `src/core/` - event reading stuff
- `src/ui/` - GTK interface
- `src/data/` - CSV import/export
- `src/utils/` - helper functions

## Issues

If you get "unknown type name 'LPWSTR'" error, make sure `windows.h` is included before `winevt.h`.

## Author

Student Name
2nd Year CSE
February 2026
