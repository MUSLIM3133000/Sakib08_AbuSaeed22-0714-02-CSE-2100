/**
 * @file ui/gtk/windows/main_window.h
 * @brief Main application window creation
 *
 * @author Team Name
 * @date February 2026
 * @version 2.0
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ui/gtk/event_viewer_context.h"

/**
 * @brief GTK "activate" signal handler — creates and shows the main window
 * @param app       The GtkApplication instance
 * @param user_data Unused (NULL)
 */
void MainWindow_Activate(GtkApplication *app, gpointer user_data);

#endif /* MAIN_WINDOW_H */
