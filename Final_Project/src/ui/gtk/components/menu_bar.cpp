/**
 * @file ui/gtk/components/menu_bar.cpp
 * @brief MenuBar implementation (C++17)
 *
 * C improvement:
 *  - g_object_unref for each GMenu managed manually in C
 *    → wrapped in a tiny RAII helper for clarity (same result, clearer intent)
 *  - NULL → nullptr throughout
 */

#include "menu_bar.h"

namespace EventViewer {

GtkWidget* MenuBar::create(EventViewerContext* ctx) {
    (void)ctx;

    GtkWidget* menubar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(menubar, "menubar");

    // Helper lambda: create a menu button and attach a GMenu to it
    auto addMenu = [&](const char* label, GMenu* menu) {
        GtkWidget* btn = gtk_menu_button_new();
        gtk_menu_button_set_label(GTK_MENU_BUTTON(btn), label);
        gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(btn), G_MENU_MODEL(menu));
        gtk_box_append(GTK_BOX(menubar), btn);
        g_object_unref(menu);   // menubar takes ownership via menu model
    };

    // File menu
    GMenu* fileMenu = g_menu_new();
    g_menu_append(fileMenu, "Open Saved Log...", "app.open_log");
    g_menu_append(fileMenu, "Save Log...",       "app.save_log");
    g_menu_append(fileMenu, "Exit",              "app.quit");
    addMenu("File", fileMenu);

    // Action menu
    GMenu* actionMenu = g_menu_new();
    g_menu_append(actionMenu, "Refresh", "app.refresh");
    addMenu("Action", actionMenu);

    // View menu
    GMenu* viewMenu = g_menu_new();
    g_menu_append(viewMenu, "Create Custom View...", "app.create_view");
    g_menu_append(viewMenu, "Import Custom View...", "app.import_view");
    addMenu("View", viewMenu);

    // Help menu
    GMenu* helpMenu = g_menu_new();
    g_menu_append(helpMenu, "About", "app.about");
    addMenu("Help", helpMenu);

    return menubar;
}

} // namespace EventViewer
