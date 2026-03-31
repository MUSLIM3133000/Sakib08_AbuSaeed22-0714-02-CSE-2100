/**
 * @file ui/gtk/components/sidebar.h
 * @brief GTK4 sidebar tree-view component (C++17)
 *
 * C improvement: Free function → static factory method, namespaced.
 */

#pragma once

#include <gtk/gtk.h>
#include "ui/gtk/event_viewer_context.h"

namespace EventViewer {

class Sidebar {
public:
    /**
     * @brief Creates the sidebar scrolled window containing the log tree view.
     * Sets ctx->treeView and ctx->treeStore.
     *
     * C equivalent: GtkWidget* Sidebar_Create(EventViewerContext* ctx);
     */
    static GtkWidget* create(EventViewerContext* ctx);
};

} // namespace EventViewer
