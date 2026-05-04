/**
 * @file view/components/tool_bar.h
 * @brief GTK4 toolbar component (C++17)
 *
 * MVC Role: VIEW — creates the application toolbar widget.
 */

#pragma once

#include <gtk/gtk.h>
#include "view/context/event_viewer_context.h"

namespace EventViewer {

class ToolBar {
public:
    static GtkWidget* create(EventViewerContext* ctx);
};

} // namespace EventViewer
