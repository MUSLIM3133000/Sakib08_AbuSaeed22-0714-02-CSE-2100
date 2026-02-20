#include "menu_bar.h"

GtkWidget *create_menu(AppData *data) {
    (void)data;

    GtkWidget *menubar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_add_css_class(menubar, "menubar");

    GtkWidget *fileBtn = gtk_menu_button_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(fileBtn), "File");
    GMenu *fileMenu = g_menu_new();
    g_menu_append(fileMenu, "Open Saved Log...", "app.open_log");
    g_menu_append(fileMenu, "Save Log...",       "app.save_log");
    g_menu_append(fileMenu, "Exit",              "app.quit");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(fileBtn), G_MENU_MODEL(fileMenu));
    gtk_box_append(GTK_BOX(menubar), fileBtn);

    GtkWidget *actionBtn = gtk_menu_button_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(actionBtn), "Action");
    GMenu *actionMenu = g_menu_new();
    g_menu_append(actionMenu, "Refresh", "app.refresh");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(actionBtn), G_MENU_MODEL(actionMenu));
    gtk_box_append(GTK_BOX(menubar), actionBtn);

    GtkWidget *viewBtn = gtk_menu_button_new();
    gtk_menu_button_set_label(GTK_MENU_BUTTON(viewBtn), "View");
    GMenu *viewMenu = g_menu_new();
    g_menu_append(viewMenu, "Create Custom View...", "app.create_view");
    g_menu_append(viewMenu, "Import Custom View...", "app.import_view");
    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(viewBtn), G_MENU_MODEL(viewMenu));
    gtk_box_append(GTK_BOX(menubar), viewBtn);

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
