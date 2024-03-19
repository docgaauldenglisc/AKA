#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h> 

typedef struct {
    char *photoloc;
    char *id;
    char *name;
    char *number;
    char *email;
    char *org;
    char *address;
} ContactText;

typedef struct {
    GtkWidget *name;
    GtkWidget *number;
    GtkWidget *email;
    GtkWidget *org;
    GtkWidget *address;
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
