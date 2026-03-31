/**
 * @file ui/gtk/components/menu_bar.h
 * @brief GTK4 menu bar component (C++17)
 *
 * C improvement: Free function → static factory method, namespaced.
 */

#pragma once

#include <gtk/gtk.h>
#include "ui/gtk/event_viewer_context.h"

namespace EventViewer {

class MenuBar {
public:
    /**
     * @brief Creates and returns the application menu bar widget.
     * @param ctx  Context pointer (reserved for future dynamic menu items).
     *
     * C equivalent: GtkWidget* MenuBar_Create(EventViewerContext* ctx);
     */
    static GtkWidget* create(EventViewerContext* ctx);
};

} // namespace EventViewer
