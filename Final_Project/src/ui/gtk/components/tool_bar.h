/**
 * @file ui/gtk/components/tool_bar.h
 * @brief GTK4 toolbar component
 *
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#ifndef TOOL_BAR_H
#define TOOL_BAR_H

#include "ui/gtk/event_viewer_context.h"

/**
 * @brief Creates the application toolbar
 * @param ctx Pointer to the shared application context
 * @return Newly created GtkWidget (toolbar)
 */
GtkWidget *ToolBar_Create(EventViewerContext *ctx);

#endif /* TOOL_BAR_H */
