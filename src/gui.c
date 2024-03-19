#include <gtk/gtk.h>

#include "gui.h"
#include "database.h"

static void remove_child_from(GtkWidget *container) {
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(container));
    if (child != NULL) {
        gtk_container_remove(GTK_CONTAINER(container), child); 
    }
}

enum {
    COL_ID = 0,
    COL_NAME,
    COL_NUMBER,
    COL_EMAIL,
    COL_ORG,
    COL_ADDRESS,
    NUM_COLS
};

static GtkTreeModel *list_create_model() {
    int max = db_max_id();

    GtkTreeIter iter;
    GtkListStore *store = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    for (int i = 1; i < max + 1; i++) {
        ContactText con;

        con.id = db_get("ID", i);
        con.name = db_get("NAME", i);
        con.number = db_get("NUMBER", i);
        con.email = db_get("EMAIL", i);
        con.org = db_get("ORG", i);
        con.address = db_get("ADDRESS", i);

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, con.id, 1, con.name, 2, con.number, 3, con.email, 4, con.org, 5, con.address, -1);
    }
    GtkTreeModel *model = GTK_TREE_MODEL(store);

    return model;
}

static GtkWidget *list_create_view() {
    GtkCellRenderer *renderer;
    GtkWidget *view = gtk_tree_view_new();

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "ID"       , renderer, "text", COL_ID, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Name"     , renderer, "text", COL_NAME, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Number"   , renderer, "text", COL_NUMBER, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Email"    , renderer, "text", COL_EMAIL, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Org"      , renderer, "text", COL_ORG, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Address"  , renderer, "text", COL_ADDRESS, NULL);
    
    GtkTreeModel *model = list_create_model();
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

    //g_object_unref(model);

    return view;
}

static void list_refresh(GtkWidget *nu, ListView *view) {
    view->model = list_create_model();

    if (gtk_tree_view_get_model(GTK_TREE_VIEW(view)) != NULL) {
        g_object_unref(gtk_tree_view_get_model(GTK_TREE_VIEW(view)));
    }


    gtk_tree_view_set_model(GTK_TREE_VIEW(view->view), view->model);
}

static void on_file_select(GtkFileChooserButton *button, gpointer data) {
    const gchar *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(button));

    gchar **location = (gchar **)data;

    *location = g_strdup(path);
}

static void save_contact(GtkWidget *nu, gpointer data) {
    Contact *con = (Contact *)data;
    con->con->name      = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->name));
    con->con->number    = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->number));
    con->con->email     = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->email));
    con->con->org       = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->org));
    con->con->address   = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->address));

    db_save_contact(con);
}

static void switch_to_new_contact_frame(GtkWidget *nu, GtkWidget *view_frame) {
    remove_child_from(view_frame);

    ContactText *text = malloc(sizeof(ContactText));
    ContactEntries *enter = malloc(sizeof(ContactEntries));
    Contact *con = malloc(sizeof(Contact));

    text->name = "";
    text->number = "";
    text->email = "";
    text->org = "";
    text->address = "";
    text->photoloc = "";

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
                    ListView *list;
                GtkWidget *view_frame;

    list = (ListView *)malloc(sizeof(ListView));

    win = gtk_application_window_new(app);
    main_frame = gtk_frame_new("AKA");
    main_grid = gtk_grid_new();
    new_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    new_contact_button = gtk_button_new_with_label("+");
    list_frame = gtk_frame_new("");
    list->view = list_create_view();
    view_frame = gtk_frame_new("");

    gtk_frame_set_label_align(GTK_FRAME(main_frame), 0.5, 1.0);

    gtk_widget_set_hexpand(new_box, TRUE);
    gtk_box_pack_start(GTK_BOX(new_box), new_contact_button, FALSE, FALSE, 0);

    list_refresh(NULL, list);

    g_signal_connect(new_contact_button, "clicked", G_CALLBACK(switch_to_new_contact_frame), view_frame);

    gtk_widget_set_hexpand(list_frame, TRUE);
    gtk_widget_set_vexpand(list_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(list_frame), 0.5, 1.0);

    gtk_widget_set_hexpand(view_frame, TRUE);
    gtk_widget_set_vexpand(view_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(view_frame), 0.5, 1.0);

    gtk_container_add(GTK_CONTAINER(list_frame), list->view);

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
