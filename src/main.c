
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sqlite3.h>

#include "conoptions.h"
#include "database.h"
#include "contactwindow.h"
#include "mainwindow.h"
#include "config.h"


int main (int argc, char *argv[]) {
    
    if (check_for_config() == 1) {
        make_config();
    }

    verify_db();


    gtk_init(&argc, &argv);

    GtkApplication *app;
    int status;

    app = gtk_application_new("com.com.com", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(main_window), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;

}
