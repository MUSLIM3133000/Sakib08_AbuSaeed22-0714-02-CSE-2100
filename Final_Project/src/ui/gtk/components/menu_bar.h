/**
 * @file ui/gtk/components/menu_bar.h
 * @brief GTK4 menu bar component
 *
 * Constructs and returns the application menu bar widget.
 * Isolated here so it can be modified without touching the
 * main window layout.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#ifndef MENU_BAR_H
#define MENU_BAR_H

#include "ui/gtk/event_viewer_context.h"

/**
 * @brief Creates the application menu bar
 * @param ctx Pointer to the shared application context
 * @return Newly created GtkWidget (menu bar); owned by the parent container
 */
GtkWidget *MenuBar_Create(EventViewerContext *ctx);

#endif /* MENU_BAR_H */
