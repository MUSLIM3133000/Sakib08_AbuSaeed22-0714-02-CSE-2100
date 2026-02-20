#ifndef EVENT_VIEWER_CONTEXT_H
#define EVENT_VIEWER_CONTEXT_H

#include <gtk/gtk.h>

#ifdef _WIN32
#include <windows.h>
#include <winevt.h>
#endif

typedef struct {
    GtkApplication *app;
    GtkWidget      *window;
    GtkWidget      *notebook;
} WindowData;

typedef struct {
    GtkWidget    *tree_view;
    GtkTreeStore *tree_store;

    GtkWidget    *admin_tree_view;
    GtkListStore *admin_store;

    GtkWidget    *recent_tree_view;
    GtkListStore *recent_store;

    GtkWidget    *log_tree_view;
    GtkListStore *log_store;

    GtkWidget    *event_details_view;
    GtkListStore *event_details_store;
} ModelData;

typedef struct {
    char *current_log_name;
#ifdef _WIN32
    EVT_HANDLE current_log_handle;
#endif
} StateData;

typedef struct {
    WindowData  window;
    ModelData   models;
    StateData   state;
} AppData;

#endif
