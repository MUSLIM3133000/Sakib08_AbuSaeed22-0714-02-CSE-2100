/**
 * @file ui/gtk/components/tool_bar.c
 * @brief GTK4 toolbar component implementation
 *
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#include "tool_bar.h"
#include "ui/gtk/callbacks/action_handlers.h"

GtkWidget *ToolBar_Create(EventViewerContext *ctx) {
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(toolbar, "toolbar");
    gtk_widget_set_margin_start(toolbar, 5);
    gtk_widget_set_margin_end(toolbar, 5);
    gtk_widget_set_margin_top(toolbar, 5);
    gtk_widget_set_margin_bottom(toolbar, 5);

    GtkWidget *backBtn = gtk_button_new_from_icon_name("go-previous");
    gtk_box_append(GTK_BOX(toolbar), backBtn);

    GtkWidget *forwardBtn = gtk_button_new_from_icon_name("go-next");
    gtk_box_append(GTK_BOX(toolbar), forwardBtn);

    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_append(GTK_BOX(toolbar), separator);

    GtkWidget *actionsBtn = gtk_menu_button_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(actionsBtn), "Actions");
    GMenu *actionsMenu = g_menu_new();
    g_menu_append(actionsMenu, "Open Saved Log...",     "app.open_log");
    g_menu_append(actionsMenu, "Create Custom View...", "app.create_view");
    g_menu_append(actionsMenu, "Import Custom View...", "app.import_view");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(actionsBtn), G_MENU_MODEL(actionsMenu));
    gtk_box_append(GTK_BOX(toolbar), actionsBtn);

    GtkWidget *refreshBtn = gtk_button_new_from_icon_name("view-refresh");
    gtk_box_append(GTK_BOX(toolbar), refreshBtn);
    g_signal_connect_swapped(refreshBtn, "clicked",
                             G_CALLBACK(ActionHandlers_OnRefresh), ctx->app);

    g_object_unref(actionsMenu);
    return toolbar;
}
