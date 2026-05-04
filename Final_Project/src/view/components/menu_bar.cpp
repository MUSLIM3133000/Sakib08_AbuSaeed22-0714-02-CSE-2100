/**
 * @file view/components/menu_bar.cpp
 * @brief MenuBar implementation (C++17)
 *
 * MVC Role: VIEW — builds the File/Action/View/Help menu structure.
 */

#include "menu_bar.h"

namespace EventViewer {

GtkWidget* MenuBar::create(EventViewerContext* ctx) {
    (void)ctx;

    GtkWidget* menubar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(menubar, "menubar");

    auto addMenu = [&](const char* label, GMenu* menu) {
        GtkWidget* btn = gtk_menu_button_new();
        gtk_menu_button_set_label(GTK_MENU_BUTTON(btn), label);
        gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(btn), G_MENU_MODEL(menu));
        gtk_box_append(GTK_BOX(menubar), btn);
        g_object_unref(menu);
    };

    GMenu* fileMenu = g_menu_new();
    g_menu_append(fileMenu, "Open Saved Log...", "app.open_log");
    g_menu_append(fileMenu, "Save Log...",       "app.save_log");
    g_menu_append(fileMenu, "Exit",              "app.quit");
    addMenu("File", fileMenu);

    GMenu* actionMenu = g_menu_new();
    g_menu_append(actionMenu, "Refresh", "app.refresh");
    addMenu("Action", actionMenu);

    GMenu* viewMenu = g_menu_new();
    g_menu_append(viewMenu, "Create Custom View...", "app.create_view");
    g_menu_append(viewMenu, "Import Custom View...", "app.import_view");
    addMenu("View", viewMenu);

    GMenu* helpMenu = g_menu_new();
    g_menu_append(helpMenu, "About", "app.about");
    addMenu("Help", helpMenu);

    return menubar;
}

} // namespace EventViewer
