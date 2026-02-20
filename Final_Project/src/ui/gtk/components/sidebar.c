#include "sidebar.h"
#include "ui/gtk/callbacks/action_handlers.h"

GtkWidget *create_sidebar(AppData *data) {
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled, TRUE);

    data->models.tree_store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_BOOLEAN);
    data->models.tree_view  = gtk_tree_view_new_with_model(
                                 GTK_TREE_MODEL(data->models.tree_store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(data->models.tree_view), FALSE);

    GtkCellRenderer     *renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn   *column   = gtk_tree_view_column_new_with_attributes(
                                        "", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(data->models.tree_view), column);

    GtkTreeSelection *selection =
        gtk_tree_view_get_selection(GTK_TREE_VIEW(data->models.tree_view));
    g_signal_connect(selection, "changed",
                     G_CALLBACK(on_tree_selection_changed), data);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled),
                                  data->models.tree_view);
    return scrolled;
}
