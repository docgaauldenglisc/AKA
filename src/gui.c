#include <gtk/gtk.h>

#include "database.h"

static void on_file_select(GtkFileChooserButton *button, gpointer data) {
    const gchar *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(button));

    gchar **location = (gchar **)data;

    *location = g_strdup(path);
}

static void remove_child_from(GtkWidget *container) {
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(container));
    if (child != NULL) {
        gtk_container_remove(GTK_CONTAINER(container), child); 
    }
}

static void switch_to_new_contact_frame(GtkWidget *nu, GtkWidget *view_frame) {
    remove_child_from(view_frame);

    GtkWidget *new_contact_grid;
        GtkWidget *name_entry;
        GtkWidget *name_label;
        GtkWidget *number_label;
        GtkWidget *number_entry;
        GtkWidget *email_label;
        GtkWidget *email_entry;
        GtkWidget *org_label;
        GtkWidget *org_entry;
        GtkWidget *address_label;
        GtkWidget *address_entry;
        GtkWidget *photoloc_label;
        GtkWidget *photoloc_entry;
        GtkWidget *save_button;

    name_entry      = gtk_entry_new();
    name_label      = gtk_label_new("Name: ");

    number_entry    = gtk_entry_new();
    number_label    = gtk_label_new("Phone Number: ");
    
    email_entry     = gtk_entry_new();
    email_label     = gtk_label_new("Email: ");

    org_entry       = gtk_entry_new();
    org_label       = gtk_label_new("Organization: ");
    
    address_entry   = gtk_entry_new();
    address_label   = gtk_label_new("Address: ");

    photoloc_entry  = gtk_file_chooser_button_new(NULL, GTK_FILE_CHOOSER_ACTION_OPEN);
    photoloc_label  = gtk_label_new("Photo: ");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(photoloc_entry), g_get_home_dir());
    gchar *file_location;
    g_signal_connect(photoloc_entry, "file-set", G_CALLBACK(on_file_select), &file_location);

    save_button     = gtk_button_new_with_label("Save");

    new_contact_grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(new_contact_grid), name_label,         0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), name_entry,         1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), number_label,       0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), number_entry,       1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), email_label,        0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), email_entry,        1, 3, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), org_label,          0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), org_entry,          1, 4, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), address_label,      0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), address_entry,      1, 5, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), photoloc_label,     0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), photoloc_entry,     1, 6, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), save_button,        2, 7, 1, 1);

    gtk_frame_set_label(GTK_FRAME(view_frame), "New Contact");
    gtk_container_add(GTK_CONTAINER(view_frame), new_contact_grid);
    gtk_widget_show_all(view_frame);
}

static void main_window(GtkApplication *app) {
    GtkWidget *win;
        GtkWidget *main_frame;
            GtkWidget *main_grid;
                GtkWidget *new_box;
                    GtkWidget *new_contact_button;
                GtkWidget *list_frame;
                GtkWidget *view_frame;

    win = gtk_application_window_new(app);
    main_frame = gtk_frame_new("AKA");
    main_grid = gtk_grid_new();
    new_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    new_contact_button = gtk_button_new_with_label("+");
    list_frame = gtk_frame_new("");
    view_frame = gtk_frame_new("");

    gtk_frame_set_label_align(GTK_FRAME(main_frame), 0.5, 1.0);

    gtk_widget_set_hexpand(new_box, TRUE);
    gtk_box_pack_start(GTK_BOX(new_box), new_contact_button, FALSE, FALSE, 0);

    g_signal_connect(new_contact_button, "clicked", G_CALLBACK(switch_to_new_contact_frame), view_frame);

    gtk_widget_set_hexpand(list_frame, TRUE);
    gtk_widget_set_vexpand(list_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(list_frame), 0.5, 1.0);

    gtk_widget_set_hexpand(view_frame, TRUE);
    gtk_widget_set_vexpand(view_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(view_frame), 0.5, 1.0);

    gtk_grid_attach(GTK_GRID(main_grid), new_box,           0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), list_frame,        0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), view_frame,        1, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(main_frame), main_grid);

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

    gtk_container_add(GTK_CONTAINER(win), main_frame);
    gtk_window_set_default_size(GTK_WINDOW(win), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_show_all(win);
}

int gui_init(int argc, char **argv) {
    GtkApplication *app;
    int status;

    gtk_init(NULL, NULL);

    app = gtk_application_new("com.com.com", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(main_window), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
