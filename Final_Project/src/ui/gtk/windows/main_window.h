/**
 * @file ui/gtk/windows/main_window.h
 * @brief Main application window (C++17)
 *
 * C improvement:
 *  - Free function MainWindow_Activate → static method MainWindow::activate
 *  - Private helper functions declared in .cpp (not in header)
 */

#pragma once

#include <gtk/gtk.h>
#include "ui/gtk/event_viewer_context.h"

namespace EventViewer {

class MainWindow {
public:
    /**
     * @brief GTK "activate" signal handler — creates and shows the main window.
     * @param app       The GtkApplication instance
     * @param userData  Pointer to pre-built EventViewerContext (injected from main)
     *
     * C equivalent: void MainWindow_Activate(GtkApplication* app, gpointer user_data);
     *
     * DIP improvement: userData carries a pre-constructed EventViewerContext
     * whose logRepository and eventExporter are already injected — main_window
     * never instantiates concrete infrastructure classes.
     */
    static void activate(GtkApplication* app, gpointer userData);
};

} // namespace EventViewer
