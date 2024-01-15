#ifndef GUI_H
#define GUI_H

// New Contact Thing
static void take_user_input(GtkWidget *entry, gpointer contact_data);
static void contact_window(GtkApplication *app, gpointer data);

// Main Window
enum {
    COL_ID = 0,
    COL_NAME,
    COL_NUMBER,
    COL_EMAIL,
    COL_ORG,
    COL_ADDRESS,
    NUM_COLS
};

static GtkTreeModel* create_mode();
static GtkWidget* create_view();
static void refresh(GtkWidget *not_used, GtkTreeView *view);
static void main_window(GtkApplication *app, gpointer data);

// AKA
start_gui(int argc, char *argv[]);

#endif
