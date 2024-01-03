#include <gtk/gtk.h>

#include "newcontactwindow.h"
#include "mainwindow.h"

int main (int	    argc,
	  char	  **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("com.com.com", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK (mainWindow), NULL);
    status = g_application_run (G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;

}
