/**
 * @file ui/gtk/components/tool_bar.h
 * @brief GTK4 toolbar component (C++17)
 *
 * C improvement: Free function → static factory method, namespaced.
 */

#pragma once

#include <gtk/gtk.h>
#include "ui/gtk/event_viewer_context.h"

namespace EventViewer {

class ToolBar {
public:
    /**
     * @brief Creates and returns the application toolbar widget.
     *
     * C equivalent: GtkWidget* ToolBar_Create(EventViewerContext* ctx);
     */
    static GtkWidget* create(EventViewerContext* ctx);
};

} // namespace EventViewer
