#include <gtk/gtk.h>

#include "gui.h"
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

static void save_contact(GtkWidget *nu, gpointer data) {
    Contact *con = (Contact *)data;
    con->con->name      = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->name));
    con->con->number    = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->number));
    con->con->email     = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->email));
    con->con->org       = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->org));
    con->con->address   = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->address));
    printf("NAME %s\n", con->con->name);
    printf("NUMBER %s\n", con->con->number);
    printf("EMAIL %s\n", con->con->email);
    printf("ORG %s\n", con->con->org);
    printf("ADDRESS %s\n", con->con->address);
    printf("PHOTO LOCATION %s\n", con->con->photoloc);

    db_save_contact(con);
}

static void switch_to_new_contact_frame(GtkWidget *nu, GtkWidget *view_frame) {
    remove_child_from(view_frame);

    ContactText *text = malloc(sizeof(ContactText));
    ContactEntries *enter = malloc(sizeof(ContactEntries));
    Contact *con = malloc(sizeof(Contact));

    text->name = "\0";
    text->number = "\0";
    text->email = "\0";
    text->org = "\0";
    text->address = "\0";
    text->photoloc = "\0";

    con->con = text;
    con->enter = enter;

    GtkWidget *new_contact_grid;
        GtkWidget *name_label;
        GtkWidget *number_label;
        GtkWidget *email_label;
        GtkWidget *org_label;
        GtkWidget *address_label;
        GtkWidget *photoloc_label;
        GtkWidget *save_button;

    enter->name     = gtk_entry_new();
    name_label      = gtk_label_new("Name: ");

    enter->number   = gtk_entry_new();
    number_label    = gtk_label_new("Phone Number: ");
    
    enter->email    = gtk_entry_new();
    email_label     = gtk_label_new("Email: ");

    enter->org      = gtk_entry_new();
    org_label       = gtk_label_new("Organization: ");
    
    enter->address  = gtk_entry_new();
    address_label   = gtk_label_new("Address: ");

    enter->photoloc = gtk_file_chooser_button_new("Photo", GTK_FILE_CHOOSER_ACTION_OPEN);
    photoloc_label  = gtk_label_new("Photo: ");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(enter->photoloc), g_get_home_dir());
    g_signal_connect(enter->photoloc, "file-set", G_CALLBACK(on_file_select), &con->con->photoloc);

    save_button = gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(save_contact), con);

    new_contact_grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(new_contact_grid), name_label,         0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->name,         1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), number_label,       0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->number,       1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), email_label,        0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->email,        1, 3, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), org_label,          0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->org,          1, 4, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), address_label,      0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->address,      1, 5, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), photoloc_label,     0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->photoloc,     1, 6, 1, 1);

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

    gtk_init(&argc, &argv);

    app = gtk_application_new("com.com.com", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(main_window), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
