#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

typedef struct {
    char *id;
    char *name;
    char *title;
    char *phone;
    char *email;
    char *org;
    char *address;
    char *extra;
    char *photoloc;
} ContactText;

typedef struct {
    GtkWidget *name;
    GtkWidget *title;
    GtkWidget *phone;
    GtkWidget *email;
    GtkWidget *org;
    GtkWidget *address;
    GtkWidget *extra;
    GtkWidget *photoloc;
} ContactEntries;

typedef struct {
    GtkTreeModel *model;
    GtkWidget *view;
} ListView;

typedef struct {
    ContactText *con;
    ContactEntries *enter;
} Contact;

int gui_init(int argc, char **argv);

#endif
