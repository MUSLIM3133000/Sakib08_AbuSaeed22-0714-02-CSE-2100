/**
 * @file ui/gtk/event_viewer_context.h
 * @brief Shared application context passed between all UI modules
 *
 * Contains all application state, GTK widget references, and data models
 * in a single unified structure.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#ifndef EVENT_VIEWER_CONTEXT_H
#define EVENT_VIEWER_CONTEXT_H

#include <gtk/gtk.h>

#ifdef _WIN32
#include <windows.h>
#include <winevt.h>
#endif

/**
 * @brief Main application context containing all state and widgets
 *
 * This structure holds all GTK widgets, data models, and application state
 * in one unified object for easy access throughout the application.
 */
typedef struct {
    /* Application and main window */
    GtkApplication *app;
    GtkWidget      *window;
    GtkWidget      *notebook;
    
    /* Tree view (sidebar) */
    GtkWidget    *tree_view;
    GtkTreeStore *tree_store;

    /* Admin events table */
    GtkWidget    *admin_tree_view;
    GtkListStore *admin_store;

    /* Recent nodes table */
    GtkWidget    *recent_tree_view;
    GtkListStore *recent_store;

    /* Log summary table */
    GtkWidget    *log_tree_view;
    GtkListStore *log_store;

    /* Event details view */
    GtkWidget    *event_details_view;
    GtkListStore *event_details_store;
    
    /* Application state */
    char *current_log_name;
#ifdef _WIN32
    EVT_HANDLE current_log_handle;
#endif
} EventViewerContext;

#endif /* EVENT_VIEWER_CONTEXT_H */
