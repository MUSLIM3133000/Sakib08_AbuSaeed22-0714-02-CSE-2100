/**
 * @file view/components/menu_bar.h
 * @brief GTK4 menu bar component (C++17)
 *
 * MVC Role: VIEW — creates the application menu bar widget.
 */

#pragma once

#include <gtk/gtk.h>
#include "view/context/event_viewer_context.h"

namespace EventViewer {

class MenuBar {
public:
    static GtkWidget* create(EventViewerContext* ctx);
};

} // namespace EventViewer
