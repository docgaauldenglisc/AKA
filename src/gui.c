#include <string.h>
#include <gtk/gtk.h>

#include "gui.h"
#include "database.h"

enum {
    COL_ID = 0, 
    COL_NAME,
    COL_TITLE,
    COL_PHONE,
    COL_EMAIL,
    COL_ORG,
    COL_ADDRESS,
    NUM_COLS
};

ListView g_list_view;
GtkWidget *g_view_frame;
GtkWidget *g_win;
ContactText g_contact;

static void gui_send_error(char *err);
static void gui_delete_contact(GtkWidget *nu, gpointer data);
static void remove_child_from(GtkWidget *container);
static void search_callback(GtkWidget *search_entry, gpointer data);
static GtkTreeModel *list_create_model();
static GtkWidget *list_create_view();
static void list_refresh();
static void gui_edit_contact(GtkWidget *nu, gpointer data);
static void switch_to_edit_contact_frame(GtkWidget *nu, gpointer nu2);
static void switch_to_view_contact_frame(GtkTreeSelection *selection, GtkWidget *view_frame);
static void gui_save_contact(GtkWidget *nu, gpointer data);
static void on_file_select(GtkFileChooserButton *button, gpointer data);
static void switch_to_new_contact_frame(GtkWidget *nu, GtkWidget *view_frame);
static void main_window(GtkApplication *app);
int gui_init(int argc, char **argv);

static void gui_send_error(char *err) {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(g_win), flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, err);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void gui_delete_contact(GtkWidget *nu, gpointer nu2) {
    db_delete_contact(atoi(g_contact.id)); 
    list_refresh();
}

static void remove_child_from(GtkWidget *container) {
    GtkWidget *child = gtk_bin_get_child(GTK_BIN(container));
    if (child != NULL) {
        gtk_container_remove(GTK_CONTAINER(container), child); 
    }
}

static void search_callback(GtkWidget *search_entry, gpointer data) {
    GtkTreeView *list_view = (GtkTreeView *)data;

    char *query = (char*)gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(search_entry)));

    if (query[0] == '\0') {
        list_refresh();
        return;
    }

    idList ids = {.ids = NULL, .id_amount = 0};

    ids = db_search(query);

    if (gtk_tree_view_get_model(GTK_TREE_VIEW(list_view)) != NULL) {
        g_object_unref(gtk_tree_view_get_model(GTK_TREE_VIEW(list_view)));
    }


    GtkTreeIter iter;

    int max = ids.id_amount;

    GtkListStore *store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    for (int i = 0; i < max; i++) {
        char *id;
        char *name;
        char *title;
        char *phone;
        char *email;
        char *org;
        char *address;

        id = db_get("ID", ids.ids[i]);
        name = db_get("NAME", ids.ids[i]);
        title = db_get("TITLE", ids.ids[i]);
        phone = db_get("PHONE", ids.ids[i]);
        email = db_get("EMAIL", ids.ids[i]);
        org = db_get("ORG", ids.ids[i]);
        address = db_get("ADDRESS", ids.ids[i]);

        gtk_list_store_append(GTK_LIST_STORE(store), &iter);
        gtk_list_store_set(store, &iter, COL_ID, id, COL_NAME, name, COL_TITLE, title, COL_PHONE, phone, COL_EMAIL, email, COL_ORG, org, COL_ADDRESS, address, -1);
    }
    GtkTreeModel *model = GTK_TREE_MODEL(store);

    gtk_tree_view_set_model(GTK_TREE_VIEW(list_view), model);
}

static GtkTreeModel *list_create_model() {
    int max = db_max_id();

    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    for (int i = 1; i < max + 1; i++) {
        char *id;
        char *name;
        char *title;
        char *phone;
        char *email;
        char *org;
        char *address;

        if (strcmp(db_get("NAME", i), "del") == 0) {
        }
        else {
            //Id is already known, so it doesn't need to be grabbed from the database
            id = malloc(sizeof(char) * 10);
            snprintf(id, 11, "%i", i);
            name = db_get("NAME", i);
            title = db_get("TITLE", i);
            phone = db_get("PHONE", i);
            email = db_get("EMAIL", i);
            org = db_get("ORG", i);
            address = db_get("ADDRESS", i);

            gtk_tree_store_append(store, &iter, NULL);
            gtk_tree_store_set(store, &iter, COL_ID, id, COL_NAME, name, COL_TITLE, title, COL_PHONE, phone, COL_EMAIL, email, COL_ORG, org, 
                    COL_ADDRESS, address, -1);
        }
    }
    GtkTreeModel *model = GTK_TREE_MODEL(store);

    return model;
}

static GtkWidget *list_create_view() {
    GtkCellRenderer *renderer;
    GtkWidget *view = gtk_tree_view_new();

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "ID"           , renderer, "text", COL_ID, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Name"         , renderer, "text", COL_NAME, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Title"        , renderer, "text", COL_TITLE, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Phone Number" , renderer, "text", COL_PHONE, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Email"        , renderer, "text", COL_EMAIL, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Org"          , renderer, "text", COL_ORG, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Address"      , renderer, "text", COL_ADDRESS, NULL);
    
    GtkTreeModel *model = list_create_model();
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

    return view;
}

static void list_refresh() {
    g_list_view.model = list_create_model();

    gtk_tree_view_set_model(GTK_TREE_VIEW(g_list_view.view), g_list_view.model);
}

static void gui_edit_contact(GtkWidget *nu, gpointer data) {
    Contact *con = (Contact *)data;
    con->con->name      = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->name));
    con->con->title     = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->title));
    con->con->phone     = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->phone));
    con->con->email     = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->email));
    con->con->org       = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->org));
    con->con->address   = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->address));
    con->con->extra     = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->extra));

    db_edit_contact(con->con);
    list_refresh();
}

static void switch_to_edit_contact_frame(GtkWidget *nu, gpointer nu2) {
    if (g_contact.id == NULL) {
        return;
    }
    remove_child_from(g_view_frame);

    ContactText *text = &g_contact;
    ContactEntries *enter = malloc(sizeof(ContactEntries));
    Contact *con = malloc(sizeof(Contact));

    con->con = text;
    con->enter = enter;

    GtkWidget *edit_contact_grid;
        GtkWidget *name_label;
        GtkEntryBuffer *name_buf;
        GtkWidget *title_label;
        GtkEntryBuffer *title_buf;
        GtkWidget *phone_label;
        GtkEntryBuffer *phone_buf;
        GtkWidget *email_label;
        GtkEntryBuffer *email_buf;
        GtkWidget *org_label;
        GtkEntryBuffer *org_buf;
        GtkWidget *address_label;
        GtkEntryBuffer *address_buf;
        GtkWidget *extra_label;
        GtkEntryBuffer *extra_buf;
        GtkWidget *photoloc_label;
        GtkWidget *save_button;

    name_buf        = gtk_entry_buffer_new(text->name, strlen(text->name));
    enter->name     = gtk_entry_new_with_buffer(name_buf);
    name_label      = gtk_label_new("Name: ");

    title_buf       = gtk_entry_buffer_new(text->title, strlen(text->title));
    enter->title    = gtk_entry_new_with_buffer(title_buf);
    title_label     = gtk_label_new("Title: ");

    phone_buf       = gtk_entry_buffer_new(text->phone, strlen(text->phone));
    enter->phone    = gtk_entry_new_with_buffer(phone_buf);
    phone_label     = gtk_label_new("Phone Number: ");
    
    email_buf       = gtk_entry_buffer_new(text->email, strlen(text->email));
    enter->email    = gtk_entry_new_with_buffer(email_buf);
    email_label     = gtk_label_new("Email: ");

    org_buf         = gtk_entry_buffer_new(text->org, strlen(text->org));
    enter->org      = gtk_entry_new_with_buffer(org_buf);
    org_label       = gtk_label_new("Organization: ");
    
    address_buf     = gtk_entry_buffer_new(text->address, strlen(text->address));
    enter->address  = gtk_entry_new_with_buffer(address_buf);
    address_label   = gtk_label_new("Address: ");

    extra_buf       = gtk_entry_buffer_new(text->address, strlen(text->extra));
    enter->extra    = gtk_entry_new_with_buffer(extra_buf);
    extra_label     = gtk_label_new("Extra Info: ");

    enter->photoloc = gtk_file_chooser_button_new(text->photoloc, GTK_FILE_CHOOSER_ACTION_OPEN);
    photoloc_label  = gtk_label_new("Photo: ");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(enter->photoloc), g_get_home_dir());
    g_signal_connect(enter->photoloc, "file-set", G_CALLBACK(on_file_select), &con->con->photoloc);

    save_button = gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(gui_edit_contact), con);

    edit_contact_grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(edit_contact_grid), name_label,         0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), enter->name,        1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(edit_contact_grid), title_label,        0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), enter->title,       1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(edit_contact_grid), phone_label,        0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), enter->phone,       1, 3, 1, 1);

    gtk_grid_attach(GTK_GRID(edit_contact_grid), email_label,        0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), enter->email,       1, 4, 1, 1);

    gtk_grid_attach(GTK_GRID(edit_contact_grid), org_label,          0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), enter->org,         1, 5, 1, 1);

    gtk_grid_attach(GTK_GRID(edit_contact_grid), address_label,      0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), enter->address,     1, 6, 1, 1);

    gtk_grid_attach(GTK_GRID(edit_contact_grid), extra_label,        0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), enter->extra,       1, 7, 1, 1);

    gtk_grid_attach(GTK_GRID(edit_contact_grid), photoloc_label,     0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), enter->photoloc,    1, 8, 1, 1);

    gtk_grid_attach(GTK_GRID(edit_contact_grid), save_button,        2, 9, 1, 1);

    gtk_frame_set_label(GTK_FRAME(g_view_frame), "Edit Contact");
    gtk_container_add(GTK_CONTAINER(g_view_frame), edit_contact_grid);
    gtk_widget_show_all(g_view_frame);
}

static void switch_to_view_contact_frame(GtkTreeSelection *selection, GtkWidget *view_frame) {
    remove_child_from(view_frame);
    GtkTreeIter iter;
    GtkTreeModel *model;
    ContactEntries c_labels;

    GtkWidget *grid;
    GtkWidget *name_label;
    GtkWidget *title_label;
    GtkWidget *phone_label;
    GtkWidget *email_label;
    GtkWidget *org_label;
    GtkWidget *address_label;
    GtkWidget *extra_label;
    GtkWidget *photo;
    GtkWidget *edit_button;
    GtkWidget *delete_button;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter, COL_ID     , &g_contact.id         , -1);
        if (g_contact.id == NULL) {
            return;
        }
        gtk_tree_model_get(model, &iter, COL_NAME   , &g_contact.name       , -1);
        gtk_tree_model_get(model, &iter, COL_TITLE  , &g_contact.title      , -1);
        gtk_tree_model_get(model, &iter, COL_PHONE  , &g_contact.phone      , -1);
        gtk_tree_model_get(model, &iter, COL_EMAIL  , &g_contact.email      , -1);
        gtk_tree_model_get(model, &iter, COL_ORG    , &g_contact.org        , -1);
        gtk_tree_model_get(model, &iter, COL_ADDRESS, &g_contact.address    , -1);
        //Get extra from database because it isn't listed
        g_contact.extra = db_get("EXTRA", atoi(g_contact.id));

        gtk_frame_set_label(GTK_FRAME(view_frame), g_contact.name);

        grid = gtk_grid_new();

        g_contact.photoloc = db_get("PHOTOLOC", atoi(g_contact.id));
        if (strcmp(g_contact.photoloc, "")) {
            photo = gtk_image_new_from_file(g_contact.photoloc);
            GdkPixbuf *buf = gtk_image_get_pixbuf(GTK_IMAGE(photo));
            int width = gdk_pixbuf_get_width(buf);
            int height = gdk_pixbuf_get_height(buf);
            double scale_width = 500.0 / width;
            double scale_height = 500.0 / height;
            double scale_factor = MIN(scale_width, scale_height);
            GdkPixbuf *scaled_buf = gdk_pixbuf_scale_simple(buf, width * scale_factor, height * scale_factor, GDK_INTERP_BILINEAR);
            photo = gtk_image_new_from_pixbuf(scaled_buf);
            gtk_grid_attach(GTK_GRID(grid), photo, 1, 0, 2, 1);
        }

        name_label      = gtk_label_new("Name: ");
        gtk_widget_set_hexpand(name_label, TRUE);
        title_label     = gtk_label_new("Title: ");
        gtk_widget_set_hexpand(title_label, TRUE);
        phone_label     = gtk_label_new("Number: ");
        gtk_widget_set_hexpand(phone_label, TRUE);
        email_label     = gtk_label_new("Email: ");
        gtk_widget_set_hexpand(email_label, TRUE);
        org_label	    = gtk_label_new("Org: ");
        gtk_widget_set_hexpand(org_label, TRUE);
        address_label   = gtk_label_new("Address: ");
        gtk_widget_set_hexpand(address_label, TRUE);
        extra_label     = gtk_label_new("Extra Info: ");
        gtk_widget_set_hexpand(extra_label, TRUE);
        c_labels.name = gtk_label_new(g_contact.name);
        gtk_widget_set_hexpand(c_labels.name, TRUE);
        c_labels.title = gtk_label_new(g_contact.title);
        gtk_widget_set_hexpand(c_labels.title, TRUE);
        c_labels.phone = gtk_label_new(g_contact.phone);
        gtk_widget_set_hexpand(c_labels.phone, TRUE);
        c_labels.email = gtk_label_new(g_contact.email);
        gtk_widget_set_hexpand(c_labels.email, TRUE);
        c_labels.org = gtk_label_new(g_contact.org);
        gtk_widget_set_hexpand(c_labels.org, TRUE);
        c_labels.address = gtk_label_new(g_contact.address);
        gtk_widget_set_hexpand(c_labels.address, TRUE);
        c_labels.extra = gtk_label_new(g_contact.extra);
        gtk_widget_set_hexpand(c_labels.extra, TRUE);
        edit_button = gtk_button_new_with_label("Edit");
        delete_button = gtk_button_new_with_label("Delete");

        g_signal_connect(edit_button, "clicked", G_CALLBACK(switch_to_edit_contact_frame), NULL);
        g_signal_connect(delete_button, "clicked", G_CALLBACK(gui_delete_contact), NULL);
        gtk_widget_set_hexpand(edit_button, TRUE);
//                                                                x, y, w, h
        gtk_grid_attach(GTK_GRID(grid), name_label              , 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), title_label             , 1, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), phone_label             , 1, 3, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), email_label             , 1, 4, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), org_label               , 1, 5, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), address_label           , 1, 6, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), extra_label             , 1, 7, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.name           , 2, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.title          , 2, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.phone          , 2, 3, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.email          , 2, 4, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.org            , 2, 5, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.address        , 2, 6, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.extra          , 2, 7, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), edit_button             , 1, 8, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), delete_button           , 2, 8, 1, 1);

        gtk_container_add(GTK_CONTAINER(view_frame), grid);
        gtk_widget_show_all(view_frame);
    }
}

static void gui_save_contact(GtkWidget *nu, gpointer data) {
    Contact *con = (Contact *)data;
    con->con->name      = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->name));
    con->con->title     = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->title));
    con->con->phone     = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->phone));
    con->con->email     = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->email));
    con->con->org       = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->org));
    con->con->address   = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->address));
    con->con->extra     = (char *)gtk_entry_get_text(GTK_ENTRY(con->enter->extra));

    switch (db_save_contact(con->con)) {
        case 0:
            //Do nothing
            break;
        case 1:
            gui_send_error("Phone number not valid");
            break;
        case 2:
            gui_send_error("Email Address not valid");
            break;
        case 3:
            gui_send_error("Address not valid");
            break;
        case 4:
            gui_send_error("Name not valid");
            break;
    }
    list_refresh();
}

static void on_file_select(GtkFileChooserButton *button, gpointer data) {
    const gchar *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(button));
    gchar **location = (gchar **)data;
    *location = g_strdup(path);
}

static void switch_to_new_contact_frame(GtkWidget *nu, GtkWidget *view_frame) {
    remove_child_from(view_frame);

    ContactText *text = malloc(sizeof(ContactText));
    ContactEntries *enter = malloc(sizeof(ContactEntries));
    Contact *con = malloc(sizeof(Contact));

    text->name = "";
    text->title = "";
    text->phone = "";
    text->email = "";
    text->org = "";
    text->address = "";
    text->extra = "";
    text->photoloc = "";

    con->con = text;
    con->enter = enter;

    GtkWidget *new_contact_grid;
        GtkWidget *name_label;
        GtkWidget *title_label;
        GtkWidget *phone_label;
        GtkWidget *email_label;
        GtkWidget *org_label;
        GtkWidget *address_label;
        GtkWidget *extra_label;
        GtkWidget *photoloc_label;
        GtkWidget *save_button;

    enter->name     = gtk_entry_new();
    name_label      = gtk_label_new("Name: ");

    enter->title    = gtk_entry_new();
    title_label     = gtk_label_new("Title: ");

    enter->phone    = gtk_entry_new();
    phone_label     = gtk_label_new("Phone Number: ");
    
    enter->email    = gtk_entry_new();
    email_label     = gtk_label_new("Email: ");

    enter->org      = gtk_entry_new();
    org_label       = gtk_label_new("Organization: ");
    
    enter->address  = gtk_entry_new();
    address_label   = gtk_label_new("Address: ");

    enter->extra    = gtk_entry_new();
    extra_label     = gtk_label_new("Extra Info: ");

    enter->photoloc = gtk_file_chooser_button_new("Photo", GTK_FILE_CHOOSER_ACTION_OPEN);
    photoloc_label  = gtk_label_new("Photo: ");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(enter->photoloc), g_get_home_dir());
    g_signal_connect(enter->photoloc, "file-set", G_CALLBACK(on_file_select), &con->con->photoloc);

    save_button = gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(gui_save_contact), con);

    new_contact_grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(new_contact_grid), name_label,         0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->name,        1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), title_label,        0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->title,       1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), phone_label,        0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->phone,       1, 3, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), email_label,        0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->email,       1, 4, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), org_label,          0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->org,         1, 5, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), address_label,      0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->address,     1, 6, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), extra_label,        0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->extra,       1, 7, 1, 1);
    
    gtk_grid_attach(GTK_GRID(new_contact_grid), photoloc_label,     0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter->photoloc,    1, 8, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), save_button,        2, 9, 1, 1);

    gtk_frame_set_label(GTK_FRAME(view_frame), "New Contact");
    gtk_container_add(GTK_CONTAINER(view_frame), new_contact_grid);
    gtk_widget_show_all(view_frame);
}

static void main_window(GtkApplication *app) {
    GtkWidget *win;
        GtkWidget *main_box;
            GtkWidget *menu_bar;
                GtkWidget *file_menu;
                    GtkWidget *file_menu_item;
                    GtkWidget *new_contact_item;
                GtkWidget *edit_menu;
                    GtkWidget *edit_menu_item;
                    GtkWidget *edit_contact_item;
            GtkWidget *main_grid;
                GtkWidget *new_box;
                    GtkWidget *new_contact_button;
                    GtkWidget *refresh_button;
                        GtkWidget *refresh_icon;
                GtkWidget *list_frame;
                    GtkWidget *list_grid;
                        GtkWidget *search_entry;
                        GtkWidget *scrolled_list_frame;
                            ListView *list;
                        GtkTreeSelection *list_selection;
                GtkWidget *view_frame;

    list = &g_list_view;

    g_win = gtk_application_window_new(app);
    win = g_win;
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    menu_bar = gtk_menu_bar_new();
    file_menu = gtk_menu_new();
    file_menu_item = gtk_menu_item_new_with_label("File");
    new_contact_item = gtk_menu_item_new_with_label("New Contact");
    edit_menu = gtk_menu_new();
    edit_menu_item = gtk_menu_item_new_with_label("Edit");
    edit_contact_item = gtk_menu_item_new_with_label("Edit Contact");
    main_grid = gtk_grid_new();
    new_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    new_contact_button = gtk_button_new_with_label("+");
    refresh_button = gtk_button_new();
    refresh_icon = gtk_image_new_from_icon_name("view-refresh-symbolic", GTK_ICON_SIZE_BUTTON);
    list_frame = gtk_frame_new("List");
    list_grid = gtk_grid_new();
    search_entry = gtk_search_entry_new();
    scrolled_list_frame = gtk_scrolled_window_new(NULL, NULL);
    list->view = list_create_view();
    g_view_frame = gtk_frame_new(NULL);
    view_frame = g_view_frame;

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), new_contact_item);

    g_signal_connect(new_contact_item, "activate", G_CALLBACK(switch_to_new_contact_frame), view_frame);

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_contact_item);

    g_signal_connect(edit_contact_item, "activate", G_CALLBACK(switch_to_edit_contact_frame), view_frame);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_menu_item);

    gtk_widget_set_hexpand(new_box, TRUE);
    gtk_box_pack_start(GTK_BOX(new_box), new_contact_button, FALSE, FALSE, 0);

    g_signal_connect(search_entry, "search-changed", G_CALLBACK(search_callback), list->view);

    g_signal_connect(new_contact_button, "clicked", G_CALLBACK(switch_to_new_contact_frame), view_frame);

    gtk_button_set_image(GTK_BUTTON(refresh_button), refresh_icon);
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(list_refresh), list);

    gtk_widget_set_hexpand(list_frame, TRUE);
    gtk_widget_set_vexpand(list_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(list_frame), 0.5, 1.0);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_list_frame), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_list_frame, TRUE);
    gtk_widget_set_hexpand(refresh_button, FALSE);
    gtk_widget_set_hexpand(search_entry, TRUE);

    list_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list->view));
    gtk_tree_selection_set_mode(GTK_TREE_SELECTION(list_selection), GTK_SELECTION_SINGLE);
    g_signal_connect(list_selection, "changed", G_CALLBACK(switch_to_view_contact_frame), view_frame);

    gtk_widget_set_hexpand(view_frame, TRUE);
    gtk_widget_set_vexpand(view_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(view_frame), 0.5, 1.0);

    //gtk_container_add(GTK_CONTAINER(list_frame), list->view);
    gtk_grid_attach(GTK_GRID(list_grid), search_entry,      0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(list_grid), refresh_button,    1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(list_grid),scrolled_list_frame,0, 1, 2, 2);
    gtk_container_add(GTK_CONTAINER(list_frame), list_grid);
    gtk_container_add(GTK_CONTAINER(scrolled_list_frame), list->view);

    gtk_grid_attach(GTK_GRID(main_grid), new_box,           0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), list_frame,        0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), view_frame,        1, 1, 1, 1);

    gtk_container_add(GTK_CONTAINER(main_box), menu_bar);

    gtk_container_add(GTK_CONTAINER(main_box), main_grid);

    const int WINDOW_WIDTH = 1280;
    const int WINDOW_HEIGHT = 720;

    gtk_container_add(GTK_CONTAINER(win), main_box);
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
