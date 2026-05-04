/**
 * @file view/components/tool_bar.cpp
 * @brief ToolBar implementation (C++17)
 *
 * MVC Role: VIEW — builds toolbar buttons and wires them to Controller actions.
 */

#include "tool_bar.h"
#include "controller/action_handlers.h"

namespace EventViewer {

GtkWidget* ToolBar::create(EventViewerContext* ctx) {
    GtkWidget* toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_add_css_class(toolbar, "toolbar");
    gtk_widget_set_margin_start (toolbar, 5);
    gtk_widget_set_margin_end   (toolbar, 5);
    gtk_widget_set_margin_top   (toolbar, 5);
    gtk_widget_set_margin_bottom(toolbar, 5);

    GtkWidget* backBtn = gtk_button_new_from_icon_name("go-previous");
    gtk_box_append(GTK_BOX(toolbar), backBtn);

    GtkWidget* forwardBtn = gtk_button_new_from_icon_name("go-next");
    gtk_box_append(GTK_BOX(toolbar), forwardBtn);

    gtk_box_append(GTK_BOX(toolbar),
                   gtk_separator_new(GTK_ORIENTATION_VERTICAL));

    GtkWidget* actionsBtn  = gtk_menu_button_new();
    GMenu*     actionsMenu = g_menu_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(actionsBtn), "Actions");
    g_menu_append(actionsMenu, "Open Saved Log...",     "app.open_log");
    g_menu_append(actionsMenu, "Create Custom View...", "app.create_view");
    g_menu_append(actionsMenu, "Import Custom View...", "app.import_view");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(actionsBtn),
                                   G_MENU_MODEL(actionsMenu));
    g_object_unref(actionsMenu);
    gtk_box_append(GTK_BOX(toolbar), actionsBtn);

    GtkWidget* refreshBtn = gtk_button_new_from_icon_name("view-refresh");
    gtk_box_append(GTK_BOX(toolbar), refreshBtn);
    g_signal_connect_swapped(refreshBtn, "clicked",
                             G_CALLBACK(ActionHandlers::onRefresh), ctx->app);

    return toolbar;
}

} // namespace EventViewer
