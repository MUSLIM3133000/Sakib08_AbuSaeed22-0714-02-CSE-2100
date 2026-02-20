/**
 * @file ui/gtk/components/menu_bar.c
 * @brief GTK4 menu bar component implementation
 *
 * Constructs File, Action, View, and Help menus and wires them
 * to GAction entries registered on the GtkApplication.
 *
 * @author EventLogReader Team
 * @date February 2026
 * @version 2.0
 */

#include "menu_bar.h"

GtkWidget *MenuBar_Create(EventViewerContext *ctx) {
    (void)ctx; /* ctx reserved for future dynamic menus */

    GtkWidget *menubar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(menubar, "menubar");

    /* ---- File menu ---- */
    GtkWidget *fileBtn = gtk_menu_button_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(fileBtn), "File");
    GMenu *fileMenu = g_menu_new();
    g_menu_append(fileMenu, "Open Saved Log...", "app.open_log");
    g_menu_append(fileMenu, "Save Log...",       "app.save_log");
    g_menu_append(fileMenu, "Exit",              "app.quit");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(fileBtn), G_MENU_MODEL(fileMenu));
    gtk_box_append(GTK_BOX(menubar), fileBtn);

    /* ---- Action menu ---- */
    GtkWidget *actionBtn = gtk_menu_button_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(actionBtn), "Action");
    GMenu *actionMenu = g_menu_new();
    g_menu_append(actionMenu, "Refresh", "app.refresh");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(actionBtn), G_MENU_MODEL(actionMenu));
    gtk_box_append(GTK_BOX(menubar), actionBtn);

    /* ---- View menu ---- */
    GtkWidget *viewBtn = gtk_menu_button_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(viewBtn), "View");
    GMenu *viewMenu = g_menu_new();
    g_menu_append(viewMenu, "Create Custom View...", "app.create_view");
    g_menu_append(viewMenu, "Import Custom View...", "app.import_view");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(viewBtn), G_MENU_MODEL(viewMenu));
    gtk_box_append(GTK_BOX(menubar), viewBtn);

    /* ---- Help menu ---- */
    GtkWidget *helpBtn = gtk_menu_button_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(helpBtn), "Help");
    GMenu *helpMenu = g_menu_new();
    g_menu_append(helpMenu, "About", "app.about");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(helpBtn), G_MENU_MODEL(helpMenu));
    gtk_box_append(GTK_BOX(menubar), helpBtn);

    g_object_unref(fileMenu);
    g_object_unref(actionMenu);
    g_object_unref(viewMenu);
    g_object_unref(helpMenu);

    return menubar;
}
