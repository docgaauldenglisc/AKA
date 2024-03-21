#include <string.h>
#include <gtk/gtk.h>

#include "gui.h"
#include "database.h"

enum {
    COL_BOOK = 0,
    COL_ID, 
    COL_NAME,
    COL_NUMBER,
    COL_EMAIL,
    COL_ORG,
    COL_ADDRESS,
    NUM_COLS
};

ListView g_list_view;

static void reallocate_size_of_frame(GtkWidget **window, GtkWidget **view_frame) {
    GtkAllocation allocation;
    gtk_widget_get_allocation(*window, &allocation);

    const int min_width = (int)(0.45 * allocation.width);    

    gtk_widget_set_size_request(GTK_WIDGET(*view_frame), min_width, -1);
}

static void remove_child_from(GtkWidget *container) {
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(container));
    if (child != NULL) {
        gtk_container_remove(GTK_CONTAINER(container), child); 
    }
}

static GtkTreeModel *list_create_model() {
    int max = db_max_id();

    GtkTreeIter parent_iter;
    GtkTreeIter child_iter;
    GtkTreeStore *store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    char *table_name = db_get_table_name();
    gtk_tree_store_append(store, &parent_iter, NULL);
    gtk_tree_store_set(store, &parent_iter, COL_BOOK, table_name, -1);
    for (int i = 1; i < max + 1; i++) {
        ContactText con;

        con.id = db_get("ID", i);
        con.name = db_get("NAME", i);
        con.number = db_get("NUMBER", i);
        con.email = db_get("EMAIL", i);
        con.org = db_get("ORG", i);
        con.address = db_get("ADDRESS", i);

        gtk_tree_store_append(store, &child_iter, &parent_iter);
        gtk_tree_store_set(store, &child_iter, 0, NULL, 1, con.id, 2, con.name, 3, con.number, 4, con.email, 5, con.org, 6, con.address, -1);
    }
    GtkTreeModel *model = GTK_TREE_MODEL(store);

    return model;
}

static GtkWidget *list_create_view() {
    GtkCellRenderer *renderer;
    GtkWidget *view = gtk_tree_view_new();

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Book"     , renderer, "text", COL_BOOK, NULL);
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

static void list_refresh() {
    g_list_view.model = list_create_model();

    gtk_tree_view_set_model(GTK_TREE_VIEW(g_list_view.view), g_list_view.model);
}

static void switch_to_view_contact_frame(GtkTreeSelection *selection, GtkWidget *view_frame) {
    remove_child_from(view_frame);
    GtkTreeIter iter;
    GtkTreeModel *model;
    ContactText *con = malloc(sizeof(ContactText));
    ContactEntries c_labels;

    GtkWidget *grid;
    GtkWidget *name_label;
    GtkWidget *number_label;
    GtkWidget *email_label;
    GtkWidget *org_label;
    GtkWidget *address_label;
    GtkWidget *photo;
    GtkWidget *edit_button;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter, COL_ID     , &con->id       , -1);
        if (con->id == NULL) {
            return;
        }
        gtk_tree_model_get(model, &iter, COL_NAME   , &con->name     , -1);
        gtk_tree_model_get(model, &iter, COL_NUMBER , &con->number   , -1);
        gtk_tree_model_get(model, &iter, COL_EMAIL  , &con->email    , -1);
        gtk_tree_model_get(model, &iter, COL_ORG    , &con->org      , -1);
        gtk_tree_model_get(model, &iter, COL_ADDRESS, &con->address  , -1);

        gtk_frame_set_label(GTK_FRAME(view_frame), con->name);

        grid = gtk_grid_new();

        con->photoloc = db_get("PHOTOLOC", atoi(con->id));
        if (strcmp(con->photoloc, "") != 0) {
            photo = gtk_image_new_from_file(con->photoloc);
            GdkPixbuf *buf = gtk_image_get_pixbuf(GTK_IMAGE(photo));
            int width = gdk_pixbuf_get_width(buf);
            int height = gdk_pixbuf_get_height(buf);
            double scale_width = 500.0 / width;
            double scale_height = 500.0 / height;
            double scale_factor = MIN(scale_width, scale_height);
            GdkPixbuf *scaled_buf = gdk_pixbuf_scale_simple(buf, width * scale_factor, height * scale_factor, GDK_INTERP_BILINEAR);
            photo = gtk_image_new_from_pixbuf(scaled_buf);
            gtk_grid_attach(GTK_GRID(grid), photo                   , 1, 0, 2, 1);
        }


        name_label      = gtk_label_new("Name: ");
        number_label    = gtk_label_new("Number: ");
        email_label     = gtk_label_new("Email: ");
        org_label	    = gtk_label_new("Org: ");
        address_label   = gtk_label_new("Address: ");
        c_labels.name= gtk_label_new(con->name);
        c_labels.number= gtk_label_new(con->number);
        c_labels.email= gtk_label_new(con->email);
        c_labels.org= gtk_label_new(con->org);
        c_labels.address= gtk_label_new(con->address);
        edit_button = gtk_button_new_with_label("Edit");

//                                                                x, y, w, h
        gtk_grid_attach(GTK_GRID(grid), name_label              , 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), number_label            , 1, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), email_label             , 1, 3, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), org_label               , 1, 4, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), address_label           , 1, 5, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.name           , 2, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.number         , 2, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.email          , 2, 3, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.org            , 2, 4, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.address        , 2, 5, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), edit_button             , 1, 6, 2, 1);

        gtk_container_add(GTK_CONTAINER(view_frame), grid);
        gtk_widget_show_all(view_frame);

        g_free(con->id);
        g_free(con->name);
        g_free(con->number);
        g_free(con->email);
        g_free(con->org);
        g_free(con->address);
    }
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
    list_refresh();
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
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->name,        1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), number_label,       0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->number,      1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), email_label,        0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->email,       1, 3, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), org_label,          0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->org,         1, 4, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), address_label,      0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->address,     1, 5, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), photoloc_label,     0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->photoloc,    1, 6, 1, 1);

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
                    GtkWidget *refresh_button;
                        GtkWidget *refresh_icon;
                GtkWidget *list_frame;
                    ListView *list;
                        GtkTreeSelection *list_selection;
                GtkWidget *view_frame;

    list = &g_list_view;

    win = gtk_application_window_new(app);
    main_frame = gtk_frame_new("AKA");
    main_grid = gtk_grid_new();
    new_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    new_contact_button = gtk_button_new_with_label("+");
    refresh_button = gtk_button_new();
    refresh_icon = gtk_image_new_from_icon_name("view-refresh-symbolic", GTK_ICON_SIZE_BUTTON);
    list_frame = gtk_frame_new(NULL);
    list->view = list_create_view();
    view_frame = gtk_frame_new(NULL);

    gtk_frame_set_label_align(GTK_FRAME(main_frame), 0.5, 1.0);

    gtk_widget_set_hexpand(new_box, TRUE);
    gtk_box_pack_start(GTK_BOX(new_box), new_contact_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(new_box), refresh_button, FALSE, FALSE, 0);

    g_signal_connect(new_contact_button, "clicked", G_CALLBACK(switch_to_new_contact_frame), view_frame);

    gtk_button_set_image(GTK_BUTTON(refresh_button), refresh_icon);
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(list_refresh), list);

    g_signal_connect(&win, "size-allocate", G_CALLBACK(reallocate_size_of_frame), &view_frame);

    gtk_widget_set_hexpand(list_frame, TRUE);
    gtk_widget_set_vexpand(list_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(list_frame), 0.5, 1.0);

    list_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list->view));
    gtk_tree_selection_set_mode(GTK_TREE_SELECTION(list_selection), GTK_SELECTION_SINGLE);
    g_signal_connect(list_selection, "changed", G_CALLBACK(switch_to_view_contact_frame), view_frame);

    gtk_widget_set_hexpand(view_frame, TRUE);
    gtk_widget_set_vexpand(view_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(view_frame), 0.5, 1.0);

    gtk_container_add(GTK_CONTAINER(list_frame), list->view);

    gtk_grid_attach(GTK_GRID(main_grid), new_box,           0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), list_frame,        0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), view_frame,        1, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(main_frame), main_grid);

    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

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
