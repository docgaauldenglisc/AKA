#include <stdbool.h>
#include <string.h>

#include <gtk/gtk.h>

#include "gui.h"
#include "help.h"
#include "database.h"

static void gui_send_error(char *err);
static void gui_delete_contact(GtkWidget *nu, gpointer data);
static void remove_child_from(GtkWidget *container);
static void search_callback(GtkWidget *search_entry, gpointer data);
static GtkTreeModel *list_create_model();
static GtkWidget *list_create_view();
static void list_refresh();
static void gui_edit_contact();
static void switch_to_edit_contact_frame();
static void switch_to_view_contact_frame(GtkTreeSelection *selection, GtkWidget *nu);
static void gui_save_contact();
static void on_file_select(GtkFileChooserButton *button, gpointer data);
static void switch_to_new_contact_frame();
static void open_backup_dialog();
static void main_window(GtkApplication *app);
int gui_init(int argc, char **argv);

enum {
    CONTACT_GOOD = 0,
    CONTACT_PHONE_BAD,
    CONTACT_EMAIL_BAD,
    CONTACT_ADDRESS_BAD,
    CONTACT_NAME_BAD
};

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

typedef struct {
    GtkTreeModel *model;
    GtkWidget *view;
} ListView;

int g_search_opt;
ListView g_list_view;
GtkWidget *g_view_frame;
GtkWidget *g_win;
ContactText g_contact;
ContactWidgets g_entries;

static void gui_send_error(char *err) {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(g_win), flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, err);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void gui_delete_contact(GtkWidget *nu, gpointer nu2) {
    if (atoi(g_contact.id) == 0) {
        return;
    }
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
    remove_child_from(GTK_WIDGET(gtk_tree_view_get_model(GTK_TREE_VIEW(list_view))));

    GtkTreeIter iter;
    //this makes eight string columns in the list
    GtkTreeStore *store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                             G_TYPE_STRING, G_TYPE_STRING);
    //TODO make sure that change to tree store doesn't cause issues
    int max = ids.id_amount;
    for (int i = 0; i < max; i++) {
        char *id = db_get("ID", ids.ids[i]);
        char *name = db_get("NAME", ids.ids[i]);
        char *title = db_get("TITLE", ids.ids[i]);
        char *phone = db_get("PHONE", ids.ids[i]);
        char *email = db_get("EMAIL", ids.ids[i]);
        char *org = db_get("ORG", ids.ids[i]);
        char *address = db_get("ADDRESS", ids.ids[i]);

        gtk_tree_store_append(GTK_TREE_STORE(store), &iter, NULL);
        gtk_tree_store_set(store, &iter, COL_ID, id, COL_NAME, name, COL_TITLE, title, COL_PHONE, phone, COL_EMAIL, email, COL_ORG, org, COL_ADDRESS, address, -1);
    }
    GtkTreeModel *model = GTK_TREE_MODEL(store);

    gtk_tree_view_set_model(GTK_TREE_VIEW(list_view), model);
}

static bool name_is_del(int i) {
    if (strcmp(db_get("NAME", i), "del") == 0) {
        return true;
    }
    else {
        return false;
    }
}

static GtkTreeModel *list_create_model() {
    int max = db_max_id();

    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    for (int i = 1; i < max + 1; i++) {
        if (name_is_del(i)) {
            //don't add it to the list
        }
        else {
            //TODO see if these can be freed
            //Id is already known, so it doesn't need to be grabbed from the database
            char *id = malloc(sizeof(char) * 11);
            snprintf(id, 11, "%i", i);
            char *name = db_get("NAME", i);
            char *title = db_get("TITLE", i);
            char *phone = db_get("PHONE", i);
            char *email = db_get("EMAIL", i);
            char *org = db_get("ORG", i);
            char *address = db_get("ADDRESS", i);

            gtk_tree_store_append(store, &iter, NULL);
            gtk_tree_store_set(store, &iter, COL_ID, id, COL_NAME, name, COL_TITLE, title,
                               COL_PHONE, phone, COL_EMAIL, email, COL_ORG, org,
                               COL_ADDRESS, address, -1);
        }
    }
    GtkTreeModel *model = GTK_TREE_MODEL(store);

    return model;
}

static GtkWidget *list_create_view() {
    GtkWidget *view = gtk_tree_view_new();
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
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

static void gui_edit_contact() {
    g_contact.name      = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.name));
    g_contact.title     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.title));
    g_contact.phone     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.phone));
    g_contact.email     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.email));
    g_contact.org       = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.org));
    g_contact.address   = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.address));
    g_contact.extra     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.extra));

    switch (db_edit_contact(&g_contact)) {
    case CONTACT_GOOD:
        list_refresh();
        gtk_frame_set_label(GTK_FRAME(g_view_frame), "View");
        break;
    case CONTACT_PHONE_BAD:
        gui_send_error("Phone number not valid");
        break;
    case CONTACT_EMAIL_BAD:
        gui_send_error("Email Address not valid");
        break;
    case CONTACT_ADDRESS_BAD:
        gui_send_error("Address not valid");
        break;
    case CONTACT_NAME_BAD:
        gui_send_error("Name not valid");
        break;
    }
}

static void switch_to_edit_contact_frame() {
    if (g_contact.id == NULL) {
        return;
    }
    remove_child_from(g_view_frame);

    GtkEntryBuffer *name_buf        = gtk_entry_buffer_new(g_contact.name, strlen(g_contact.name));
    g_entries.name     = gtk_entry_new_with_buffer(name_buf);
    GtkWidget *name_label      = gtk_label_new("Name: ");

    GtkEntryBuffer *title_buf       = gtk_entry_buffer_new(g_contact.title, strlen(g_contact.title));
    g_entries.title    = gtk_entry_new_with_buffer(title_buf);
    GtkWidget *title_label     = gtk_label_new("Title: ");

    GtkEntryBuffer *phone_buf       = gtk_entry_buffer_new(g_contact.phone, strlen(g_contact.phone));
    g_entries.phone    = gtk_entry_new_with_buffer(phone_buf);
    GtkWidget *phone_label     = gtk_label_new("Phone Number: ");
    
    GtkEntryBuffer *email_buf       = gtk_entry_buffer_new(g_contact.email, strlen(g_contact.email));
    g_entries.email    = gtk_entry_new_with_buffer(email_buf);
    GtkWidget *email_label     = gtk_label_new("Email: ");

    GtkEntryBuffer *org_buf         = gtk_entry_buffer_new(g_contact.org, strlen(g_contact.org));
    g_entries.org      = gtk_entry_new_with_buffer(org_buf);
    GtkWidget *org_label       = gtk_label_new("Organization: ");
    
    GtkEntryBuffer *address_buf     = gtk_entry_buffer_new(g_contact.address, strlen(g_contact.address));
    g_entries.address  = gtk_entry_new_with_buffer(address_buf);
    GtkWidget *address_label   = gtk_label_new("Address: ");

    GtkEntryBuffer *extra_buf       = gtk_entry_buffer_new(g_contact.address, strlen(g_contact.extra));
    g_entries.extra    = gtk_entry_new_with_buffer(extra_buf);
    GtkWidget *extra_label     = gtk_label_new("Extra Info: ");

    g_entries.photoloc = gtk_file_chooser_button_new(g_contact.photoloc, GTK_FILE_CHOOSER_ACTION_OPEN);
    GtkWidget *photoloc_label  = gtk_label_new("Photo: ");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(g_contact.photoloc), g_get_home_dir());
    g_signal_connect(g_entries.photoloc, "file-set", G_CALLBACK(on_file_select), &g_contact.photoloc);

    GtkWidget *save_button = gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(gui_edit_contact), NULL);

    GtkWidget *edit_contact_grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(edit_contact_grid), name_label,            0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.name,        1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), title_label,           0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.title,       1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), phone_label,           0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.phone,       1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), email_label,           0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.email,       1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), org_label,             0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.org,         1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), address_label,         0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.address,     1, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), extra_label,           0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.extra,       1, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), photoloc_label,        0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.photoloc,    1, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), save_button,           2, 9, 1, 1);

    char *frame_str = malloc(sizeof(char) * 20);
    snprintf(frame_str, 20, "Edit %s", g_contact.name);

    gtk_frame_set_label(GTK_FRAME(g_view_frame), frame_str);
    gtk_container_add(GTK_CONTAINER(g_view_frame), edit_contact_grid);
    gtk_widget_show_all(g_view_frame);
}

static void set_up_photo(GtkWidget *photo) {
    GdkPixbuf *buf = gtk_image_get_pixbuf(GTK_IMAGE(photo));
    int width = gdk_pixbuf_get_width(buf);
    int height = gdk_pixbuf_get_height(buf);
    double scale_width = 500.0 / width;
    double scale_height = 500.0 / height;
    double scale_factor = MIN(scale_width, scale_height);
    GdkPixbuf *scaled_buf = gdk_pixbuf_scale_simple(buf, width * scale_factor, height * scale_factor, GDK_INTERP_BILINEAR);
    photo = gtk_image_new_from_pixbuf(scaled_buf);
}

static void switch_to_view_contact_frame(GtkTreeSelection *selection, GtkWidget *nu) {
    remove_child_from(g_view_frame);

    GtkTreeIter iter;
    GtkTreeModel *model;
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

        gtk_frame_set_label(GTK_FRAME(g_view_frame), g_contact.name);

        g_contact.photoloc = db_get("PHOTOLOC", atoi(g_contact.id));
        GtkWidget *grid = gtk_grid_new();
        if (strcmp(g_contact.photoloc, "\0")) {
            GtkWidget *photo = gtk_image_new_from_file(g_contact.photoloc);
            set_up_photo(photo);
            gtk_grid_attach(GTK_GRID(grid), photo, 1, 0, 2, 1);
        }
        GtkWidget *name_label       = gtk_label_new("Name: ");
        gtk_widget_set_hexpand(name_label, TRUE);
        GtkWidget *title_label      = gtk_label_new("Title: ");
        gtk_widget_set_hexpand(title_label, TRUE);
        GtkWidget *phone_label      = gtk_label_new("Number: ");
        gtk_widget_set_hexpand(phone_label, TRUE);
        GtkWidget *email_label      = gtk_label_new("Email: ");
        gtk_widget_set_hexpand(email_label, TRUE);
        GtkWidget *org_label        = gtk_label_new("Org: ");
        gtk_widget_set_hexpand(org_label, TRUE);
        GtkWidget *address_label    = gtk_label_new("Address: ");
        gtk_widget_set_hexpand(address_label, TRUE);
        GtkWidget *extra_label      = gtk_label_new("Extra Info: ");
        gtk_widget_set_hexpand(extra_label, TRUE);

        ContactWidgets c_labels;
        GtkWidget *edit_button;
        GtkWidget *delete_button;
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

        gtk_container_add(GTK_CONTAINER(g_view_frame), grid);
        gtk_widget_show_all(g_view_frame);
    }
}

static void gui_save_contact() {
    g_contact.name      = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.name));
    g_contact.title     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.title));
    g_contact.phone     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.phone));
    g_contact.email     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.email));
    g_contact.org       = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.org));
    g_contact.address   = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.address));
    g_contact.extra     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.extra));

    switch (db_save_contact(&g_contact)) {
    case CONTACT_GOOD:
        list_refresh();
        gtk_frame_set_label(GTK_FRAME(g_view_frame), "View");
        break;
    case CONTACT_PHONE_BAD:
        gui_send_error("Phone number not valid");
        break;
    case CONTACT_EMAIL_BAD:
        gui_send_error("Email Address not valid");
        break;
    case CONTACT_ADDRESS_BAD:
        gui_send_error("Address not valid");
        break;
    case CONTACT_NAME_BAD:
        gui_send_error("Name not valid");
        break;
    }
}

static void on_file_select(GtkFileChooserButton *button, gpointer data) {
    const gchar *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(button));
    gchar **location = (gchar **)data;
    *location = g_strdup(path);
}

static void switch_to_new_contact_frame() {
    remove_child_from(g_view_frame);

    g_contact.name = "";
    g_contact.title = "";
    g_contact.phone = "";
    g_contact.email = "";
    g_contact.org = "";
    g_contact.address = "";
    g_contact.extra = "";
    g_contact.photoloc = "";

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

    g_entries.name     = gtk_entry_new();
    name_label      = gtk_label_new("Name: ");

    g_entries.title    = gtk_entry_new();
    title_label     = gtk_label_new("Title: ");

    g_entries.phone    = gtk_entry_new();
    phone_label     = gtk_label_new("Phone Number: ");
    
    g_entries.email    = gtk_entry_new();
    email_label     = gtk_label_new("Email: ");

    g_entries.org      = gtk_entry_new();
    org_label       = gtk_label_new("Organization: ");
    
    g_entries.address  = gtk_entry_new();
    address_label   = gtk_label_new("Address: ");

    g_entries.extra    = gtk_entry_new();
    extra_label     = gtk_label_new("Extra Info: ");

    g_entries.photoloc = gtk_file_chooser_button_new("Photo", GTK_FILE_CHOOSER_ACTION_OPEN);
    photoloc_label  = gtk_label_new("Photo: ");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(g_entries.photoloc), g_get_home_dir());
    g_signal_connect(g_entries.photoloc, "file-set", G_CALLBACK(on_file_select), &g_contact.photoloc);

    save_button = gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(gui_save_contact), NULL);

    new_contact_grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(new_contact_grid), name_label,         0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.name,     1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), title_label,        0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.title,    1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), phone_label,        0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.phone,    1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), email_label,        0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.email,    1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), org_label,          0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.org,      1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), address_label,      0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.address,  1, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), extra_label,        0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.extra,    1, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), photoloc_label,     0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.photoloc, 1, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), save_button,        2, 9, 1, 1);

    gtk_frame_set_label(GTK_FRAME(g_view_frame), "New Contact");
    gtk_container_add(GTK_CONTAINER(g_view_frame), new_contact_grid);
    gtk_widget_show_all(g_view_frame);
}

static void open_backup_dialog() {
    GtkWidget *dialog;
        GtkFileChooser *chooser;
        GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
        gint res;

    dialog = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(g_win), action, ("_Cancel"),
                                         GTK_RESPONSE_CANCEL, ("_Save"), GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);
    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
    gtk_file_chooser_set_current_folder(chooser, g_get_home_dir());

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;

        filename = gtk_file_chooser_get_filename(chooser);
        db_backup_at(filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void main_window(GtkApplication *app) {
    //Prevents a segfault if the user presses "Delete Contact" in the edit menu
    //before selecting one.
    g_contact.id = "0";

    //g_win;
        GtkWidget *main_box;
            GtkWidget *menu_bar;
                GtkWidget *file_menu;
                    GtkWidget *file_menu_item;
                    GtkWidget *new_contact_item;
                    GtkWidget *backup_item;
                GtkWidget *edit_menu;
                    GtkWidget *edit_menu_item;
                    GtkWidget *edit_contact_item;
                    GtkWidget *delete_contact_item;
                GtkWidget *help_menu_item;
            GtkWidget *main_grid;
                GtkWidget *new_box;
                    GtkWidget *new_contact_button;
                    GtkWidget *refresh_button;
                        GtkWidget *refresh_icon;
                GtkWidget *list_frame;
                    GtkWidget *list_grid;
                        GtkWidget *search_entry;
                        GtkWidget *scrolled_list_frame;
                            //g_list_view;
                        GtkTreeSelection *list_selection;

    file_menu_item = gtk_menu_item_new_with_label("File");
    file_menu = gtk_menu_new();
    new_contact_item = gtk_menu_item_new_with_label("New Contact");
    backup_item = gtk_menu_item_new_with_label("Backup Contacts");
    g_view_frame = gtk_frame_new("View");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_menu_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), backup_item);
    g_signal_connect(backup_item, "activate", G_CALLBACK(open_backup_dialog), NULL);

    edit_menu = gtk_menu_new();
    edit_menu_item = gtk_menu_item_new_with_label("Edit");
    edit_contact_item = gtk_menu_item_new_with_label("Edit Contact");
    delete_contact_item = gtk_menu_item_new_with_label("Delete Contact");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), new_contact_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_contact_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), delete_contact_item);
    g_signal_connect(new_contact_item, "activate", G_CALLBACK(switch_to_new_contact_frame), NULL);
    g_signal_connect(edit_contact_item, "activate", G_CALLBACK(switch_to_edit_contact_frame), NULL);
    g_signal_connect(delete_contact_item, "activate", G_CALLBACK(gui_delete_contact), NULL);

    help_menu_item = gtk_menu_item_new_with_label("Help");
    g_signal_connect(help_menu_item, "activate", G_CALLBACK(help_open_window), NULL);

    menu_bar = gtk_menu_bar_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_menu_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_menu_item);

    new_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    new_contact_button = gtk_button_new_with_label("+");
    gtk_widget_set_hexpand(new_box, TRUE);
    gtk_box_pack_start(GTK_BOX(new_box), new_contact_button, FALSE, FALSE, 0);

    search_entry = gtk_search_entry_new();
    g_list_view.view = list_create_view();
    g_signal_connect(search_entry, "search-changed", G_CALLBACK(search_callback), g_list_view.view);

    g_signal_connect(new_contact_button, "clicked", G_CALLBACK(switch_to_new_contact_frame), g_view_frame);

    refresh_button = gtk_button_new();
    refresh_icon = gtk_image_new_from_icon_name("view-refresh-symbolic", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(refresh_button), refresh_icon);
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(list_refresh), NULL);

    list_frame = gtk_frame_new("List");
    gtk_widget_set_hexpand(list_frame, TRUE);
    gtk_widget_set_vexpand(list_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(list_frame), 0.5, 1.0);

    scrolled_list_frame = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_list_frame), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scrolled_list_frame, TRUE);
    gtk_widget_set_hexpand(refresh_button, FALSE);
    gtk_widget_set_hexpand(search_entry, TRUE);

    list_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(g_list_view.view));
    gtk_tree_selection_set_mode(GTK_TREE_SELECTION(list_selection), GTK_SELECTION_SINGLE);
    g_signal_connect(list_selection, "changed", G_CALLBACK(switch_to_view_contact_frame), NULL);

    gtk_widget_set_hexpand(g_view_frame, TRUE);
    gtk_widget_set_vexpand(g_view_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(g_view_frame), 0.5, 1.0);

    list_grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(list_grid), search_entry           , 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(list_grid), refresh_button         , 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(list_grid), scrolled_list_frame    , 0, 1, 2, 1);
    gtk_container_add(GTK_CONTAINER(list_frame), list_grid);
    gtk_container_add(GTK_CONTAINER(scrolled_list_frame), g_list_view.view);

    main_grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(main_grid), new_box,           0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), list_frame,        0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), g_view_frame,        1, 1, 1, 1);

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(main_box), menu_bar);

    gtk_container_add(GTK_CONTAINER(main_box), main_grid);

    g_win = gtk_application_window_new(app);
    gtk_container_add(GTK_CONTAINER(g_win), main_box);
    gtk_window_set_default_size(GTK_WINDOW(g_win), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_show_all(g_win);
}

int gui_init(int argc, char **argv) {
    gtk_init(&argc, &argv);

    GtkApplication *app;
    int status;
    app = gtk_application_new("com.com.com", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(main_window), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
