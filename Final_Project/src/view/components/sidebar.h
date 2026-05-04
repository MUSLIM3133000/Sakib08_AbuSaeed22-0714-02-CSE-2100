/**
 * @file view/components/sidebar.h
 * @brief GTK4 sidebar tree-view component (C++17)
 *
 * MVC Role: VIEW — creates the sidebar navigation tree widget.
 */

#pragma once

#include <gtk/gtk.h>
#include "view/context/event_viewer_context.h"

namespace EventViewer {

class Sidebar {
public:
    static GtkWidget* create(EventViewerContext* ctx);
};

} // namespace EventViewer
