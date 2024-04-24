#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *name;
    GtkWidget *title;
    GtkWidget *phone;
    GtkWidget *email;
    GtkWidget *org;
    GtkWidget *address;
    GtkWidget *website;
    GtkWidget *extra;
    GtkWidget *photoloc;
} ContactWidgets;

static const int WINDOW_WIDTH = 1280;
static const int WINDOW_HEIGHT = 720;

int gui_init(int argc, char **argv);

#endif
