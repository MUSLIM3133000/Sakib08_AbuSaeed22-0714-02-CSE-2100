/**
 * @file view/windows/main_window.h
 * @brief Main application window (C++17)
 *
 * MVC Role: VIEW — creates and displays the main GTK window.
 */

#pragma once

#include <gtk/gtk.h>
#include "view/context/event_viewer_context.h"

namespace EventViewer {

class MainWindow {
public:
    /**
     * @brief GTK "activate" signal handler — creates and shows the main window.
     * @param app       The GtkApplication instance
     * @param userData  Pointer to pre-built EventViewerContext (injected from main)
     */
    static void activate(GtkApplication* app, gpointer userData);
};

} // namespace EventViewer
