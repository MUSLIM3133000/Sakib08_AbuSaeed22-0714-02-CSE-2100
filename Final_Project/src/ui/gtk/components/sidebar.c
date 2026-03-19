/**
 * @file ui/gtk/components/sidebar.h
 * @brief GTK4 sidebar tree-view component
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "ui/gtk/event_viewer_context.h"

/**
 * @brief Creates the left-hand sidebar with the log hierarchy tree
 * @param ctx Pointer to the shared application context
 *            (sets ctx->tree_view and ctx->tree_store)
 * @return Newly created GtkWidget (scrolled window containing the tree)
 */
GtkWidget *Sidebar_Create(EventViewerContext *ctx);

#endif /* SIDEBAR_H */
