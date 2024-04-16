//Standards
#include <stdbool.h>
#include <string.h>

//Libraries
#include <gtk/gtk.h>

//Local files
#include "gui.h"
#include "help.h"
#include "database.h"

static void alloc_frame_size();
static void gui_send_error(char *err);
static void gui_delete_contact();
static void remove_child_from(GtkWidget *container);
static GtkTreeModel *list_create_model();
static GtkWidget *list_create_view();
static void list_refresh();
static void set_search_col(GtkWidget *search_opts_box);
static void search_callback(GtkWidget *search_entry);
static bool name_is_del(int i);
static void set_up_photo(GtkWidget *photo);
static void on_file_select(GtkFileChooserButton *button, gpointer data);
static void gui_save_contact();
static void gui_edit_contact();
static void switch_to_edit_contact_frame();
static void switch_to_view_contact_frame(GtkTreeSelection *selection);
static void switch_to_new_contact_frame();
static void open_backup_dialog();
static void setup_title_bar(GtkWidget *main_grid);
static void setup_action_box(GtkWidget *main_grid);
static void setup_list_frame(GtkWidget *main_grid);
static void setup_view_frame(GtkWidget *main_grid);
static void setup_main_window(GtkApplication *app);
int gui_init(int argc, char **argv);

enum {
    CONTACT_GOOD = 0,
    CONTACT_PHONE_BAD,
    CONTACT_EMAIL_BAD,
    CONTACT_ADDRESS_BAD,
    CONTACT_NAME_BAD,
    CONTACT_WEBSITE_BAD,
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
char *g_search_col;

static void alloc_frame_size() {
    GtkAllocation win_allocation;
    gtk_widget_get_allocation(g_win, &win_allocation);
    int min_width = (int)(0.45 * win_allocation.width);
    gtk_widget_set_size_request(GTK_WIDGET(g_view_frame), min_width, -1);
}

static void gui_send_error(char *err) {
    GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(g_win), flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, err);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

static void gui_delete_contact() {
    if (!g_contact.id) {
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

static GtkTreeModel *list_create_model() {
    int max = db_max_id();

    GtkTreeIter iter;
    GtkTreeStore *store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    for (int i = 1; i < max + 1; i++) {
        if (name_is_del(i)) {
            //don't add it to the list
        }
        else {
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
            free(id);
            free(name);
            free(title);
            free(phone);
            free(email);
            free(org);
            free(address);
        }
    }
    GtkTreeModel *model = GTK_TREE_MODEL(store);

    return model;
}

static GtkWidget *list_create_view() {
    GtkWidget *view = gtk_tree_view_new();
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
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
    alloc_frame_size();
    g_list_view.model = list_create_model();
    gtk_tree_view_set_model(GTK_TREE_VIEW(g_list_view.view), g_list_view.model);
}

static void set_search_col(GtkWidget *search_opts_box) {
    char *col_to_set_as = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(search_opts_box));
    unsigned long length = strlen(col_to_set_as);
    g_search_col = malloc(sizeof(char) * length);
    g_search_col = strndup(col_to_set_as, length);
}

static void search_callback(GtkWidget *search_entry) {
    char *query = (char*)gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(search_entry)));
    if (query[0] == '\0') {
        list_refresh();
        return;
    }

    idList ids = {.ids = NULL, .id_amount = 0};
    ids = db_search(query, g_search_col);

    if (gtk_tree_view_get_model(GTK_TREE_VIEW(g_list_view.view)) != NULL) {
        g_object_unref(gtk_tree_view_get_model(GTK_TREE_VIEW(g_list_view.view)));
    }
    remove_child_from(GTK_WIDGET(gtk_tree_view_get_model(GTK_TREE_VIEW(g_list_view.view))));

    GtkTreeIter iter;
    //this makes eight string columns in the list
    GtkTreeStore *store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                                             G_TYPE_STRING, G_TYPE_STRING);
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
        gtk_tree_store_set(store, &iter, COL_ID, id, COL_NAME, name, COL_TITLE, title, COL_PHONE,
                phone, COL_EMAIL, email, COL_ORG, org, COL_ADDRESS, address, -1);

        free(id);
        free(name);
        free(title);
        free(phone);
        free(email);
        free(org);
        free(address);
    }
    GtkTreeModel *model = GTK_TREE_MODEL(store);

    gtk_tree_view_set_model(GTK_TREE_VIEW(g_list_view.view), model);
}

static bool name_is_del(int i) {
    if (strcmp(db_get("NAME", i), "del") == 0) {
        return true;
    }
    else {
        return false;
    }
}

static void set_up_photo(GtkWidget *photo) {
    GdkPixbuf *buf = gtk_image_get_pixbuf(GTK_IMAGE(photo));
    int width = gdk_pixbuf_get_width(buf);
    int height = gdk_pixbuf_get_height(buf);

    int window_width;
    int window_height;
    gtk_window_get_size(GTK_WINDOW(g_win), &window_width, &window_height);

    int width_goal = 0.35 * window_width;
    int height_goal = 0.35 * window_height;
    double width_after;
    double height_after;
    if (width > width_goal || height > height_goal) {
        double scale_factor = MIN(((double)width_goal / width), ((double)height_goal / width));
        width_after = width * scale_factor;
        height_after = height * scale_factor;
    }
    else {
        width_after = width;
        height_after = height;
    }

    GdkPixbuf *scaled_buf = gdk_pixbuf_scale_simple(buf, width_after, height_after, GDK_INTERP_BILINEAR);
    gtk_image_set_from_pixbuf(GTK_IMAGE(photo), scaled_buf);
}

static void on_file_select(GtkFileChooserButton *button, gpointer data) {
    const gchar *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(button));
    gchar **location = (gchar **)data;
    *location = g_strdup(path);
}

static void gui_save_contact() {
    g_contact.name      = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.name));
    g_contact.title     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.title));
    g_contact.phone     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.phone));
    g_contact.email     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.email));
    g_contact.org       = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.org));
    g_contact.address   = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.address));
    g_contact.website   = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.website));
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
    case CONTACT_WEBSITE_BAD:
        gui_send_error("Website not valid");
        break;
    }
}

static void gui_edit_contact() {
    g_contact.name      = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.name));
    g_contact.title     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.title));
    g_contact.phone     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.phone));
    g_contact.email     = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.email));
    g_contact.org       = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.org));
    g_contact.address   = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.address));
    g_contact.website   = (char *)gtk_entry_get_text(GTK_ENTRY(g_entries.website));
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
    case CONTACT_WEBSITE_BAD:
        gui_send_error("Website not valid");
        break;
    }
}

static void switch_to_edit_contact_frame() {
    alloc_frame_size();
    if (!g_contact.id) {
        return;
    }
    remove_child_from(g_view_frame);

    GtkEntryBuffer *name_buf    = gtk_entry_buffer_new(g_contact.name, strlen(g_contact.name));
    g_entries.name              = gtk_entry_new_with_buffer(name_buf);
    GtkWidget *name_label       = gtk_label_new("Name: ");
    gtk_label_set_xalign(GTK_LABEL(name_label), 1);
    gtk_widget_set_hexpand(g_entries.name, TRUE);

    GtkEntryBuffer *title_buf   = gtk_entry_buffer_new(g_contact.title, strlen(g_contact.title));
    g_entries.title             = gtk_entry_new_with_buffer(title_buf);
    GtkWidget *title_label      = gtk_label_new("Title: ");
    gtk_label_set_xalign(GTK_LABEL(title_label), 1);
    gtk_widget_set_hexpand(g_entries.title, TRUE);

    GtkEntryBuffer *phone_buf   = gtk_entry_buffer_new(g_contact.phone, strlen(g_contact.phone));
    g_entries.phone             = gtk_entry_new_with_buffer(phone_buf);
    GtkWidget *phone_label      = gtk_label_new("Phone Number: ");
    gtk_label_set_xalign(GTK_LABEL(phone_label), 1);
    gtk_widget_set_hexpand(g_entries.phone, TRUE);
    
    GtkEntryBuffer *email_buf   = gtk_entry_buffer_new(g_contact.email, strlen(g_contact.email));
    g_entries.email             = gtk_entry_new_with_buffer(email_buf);
    GtkWidget *email_label      = gtk_label_new("Email: ");
    gtk_label_set_xalign(GTK_LABEL(email_label), 1);
    gtk_widget_set_hexpand(g_entries.email, TRUE);

    GtkEntryBuffer *org_buf     = gtk_entry_buffer_new(g_contact.org, strlen(g_contact.org));
    g_entries.org               = gtk_entry_new_with_buffer(org_buf);
    GtkWidget *org_label        = gtk_label_new("Organization: ");
    gtk_label_set_xalign(GTK_LABEL(org_label), 1);
    gtk_widget_set_hexpand(g_entries.org, TRUE);
    
    GtkEntryBuffer *address_buf = gtk_entry_buffer_new(g_contact.address, strlen(g_contact.address));
    g_entries.address           = gtk_entry_new_with_buffer(address_buf);
    GtkWidget *address_label    = gtk_label_new("Address: ");
    gtk_label_set_xalign(GTK_LABEL(address_label), 1);
    gtk_widget_set_hexpand(g_entries.address, TRUE);

    GtkEntryBuffer *website_buf = gtk_entry_buffer_new(g_contact.website, strlen(g_contact.website));
    g_entries.website           = gtk_entry_new_with_buffer(website_buf);
    GtkWidget *website_label    = gtk_label_new("Website: ");
    gtk_label_set_xalign(GTK_LABEL(website_label), 1);
    gtk_widget_set_hexpand(g_entries.website, TRUE);

    GtkEntryBuffer *extra_buf   = gtk_entry_buffer_new(g_contact.extra, strlen(g_contact.extra));
    g_entries.extra             = gtk_entry_new_with_buffer(extra_buf);
    GtkWidget *extra_label      = gtk_label_new("Extra Info: ");
    gtk_label_set_xalign(GTK_LABEL(extra_label), 1);
    gtk_widget_set_hexpand(g_entries.extra, TRUE);
    gtk_widget_set_vexpand(g_entries.extra, TRUE);

    g_entries.photoloc = gtk_file_chooser_button_new("Photo Location", GTK_FILE_CHOOSER_ACTION_OPEN);
    if (strcmp(g_contact.photoloc, "") == 0) {
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(g_entries.photoloc), g_contact.photoloc);
    }
    else {
        g_entries.photoloc = gtk_file_chooser_button_new(g_contact.photoloc, GTK_FILE_CHOOSER_ACTION_OPEN);
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(g_entries.photoloc), g_get_home_dir());
    }
    GtkWidget *photoloc_label  = gtk_label_new("Photo: ");
    gtk_label_set_xalign(GTK_LABEL(photoloc_label), 1);
    gtk_widget_set_hexpand(g_entries.photoloc, TRUE);
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
    gtk_grid_attach(GTK_GRID(edit_contact_grid), website_label,         0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.website,     1, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), extra_label,           0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.extra,       1, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), photoloc_label,        0, 9, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), g_entries.photoloc,    1, 9, 1, 1);
    gtk_grid_attach(GTK_GRID(edit_contact_grid), save_button,           0,10, 2, 1);
    gtk_grid_set_column_spacing(GTK_GRID(edit_contact_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(edit_contact_grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(edit_contact_grid), 5);

    char *frame_str = malloc(sizeof(char) * 20);
    snprintf(frame_str, 20, "Edit %s", g_contact.name);

    gtk_frame_set_label(GTK_FRAME(g_view_frame), frame_str);
    gtk_container_add(GTK_CONTAINER(g_view_frame), edit_contact_grid);
    gtk_widget_show_all(g_view_frame);

    free(frame_str);
}

static void switch_to_view_contact_frame(GtkTreeSelection *selection) {
    remove_child_from(g_view_frame);
    alloc_frame_size();

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
        //Get extra and website from database because they aren't listed
        g_contact.website = db_get("WEBSITE", atoi(g_contact.id));
        g_contact.extra = db_get("EXTRA", atoi(g_contact.id));

        gtk_frame_set_label(GTK_FRAME(g_view_frame), g_contact.name);

        g_contact.photoloc = db_get("PHOTOLOC", atoi(g_contact.id));
        GtkWidget *grid = gtk_grid_new();
        if (strcmp(g_contact.photoloc, "\0")) {
            GtkWidget *photo = gtk_image_new_from_file(g_contact.photoloc);
            set_up_photo(photo);
            gtk_widget_set_hexpand(photo, TRUE);
            gtk_grid_attach(GTK_GRID(grid), photo, 1, 0, 2, 1);
        }
        GtkWidget *name_label       = gtk_label_new("Name: ");
        gtk_label_set_xalign(GTK_LABEL(name_label), 1);
        gtk_widget_set_vexpand(name_label, TRUE);
        GtkWidget *title_label      = gtk_label_new("Title: ");
        gtk_label_set_xalign(GTK_LABEL(title_label), 1);
        gtk_widget_set_vexpand(title_label, TRUE);
        GtkWidget *phone_label      = gtk_label_new("Number: ");
        gtk_label_set_xalign(GTK_LABEL(phone_label), 1);
        gtk_widget_set_vexpand(phone_label, TRUE);
        GtkWidget *email_label      = gtk_label_new("Email: ");
        gtk_label_set_xalign(GTK_LABEL(email_label), 1);
        gtk_widget_set_vexpand(email_label, TRUE);
        GtkWidget *org_label        = gtk_label_new("Org: ");
        gtk_label_set_xalign(GTK_LABEL(org_label), 1);
        gtk_widget_set_vexpand(org_label, TRUE);
        GtkWidget *address_label    = gtk_label_new("Address: ");
        gtk_label_set_xalign(GTK_LABEL(address_label), 1);
        gtk_widget_set_vexpand(address_label, TRUE);
        GtkWidget *website_label    = gtk_label_new("Website: ");
        gtk_label_set_xalign(GTK_LABEL(website_label), 1);
        gtk_widget_set_vexpand(website_label, TRUE);
        GtkWidget *extra_label      = gtk_label_new("Extra Info: ");
        gtk_label_set_xalign(GTK_LABEL(extra_label), 1);
        gtk_widget_set_vexpand(extra_label, TRUE);

        ContactWidgets c_labels;
        c_labels.name = gtk_label_new(g_contact.name);
        gtk_label_set_xalign(GTK_LABEL(c_labels.name), 0);
        gtk_widget_set_hexpand(c_labels.name, TRUE);
        c_labels.title = gtk_label_new(g_contact.title);
        gtk_label_set_xalign(GTK_LABEL(c_labels.title), 0);
        c_labels.phone = gtk_label_new(g_contact.phone);
        gtk_label_set_xalign(GTK_LABEL(c_labels.phone), 0);
        c_labels.email = gtk_label_new(g_contact.email);
        gtk_label_set_xalign(GTK_LABEL(c_labels.email), 0);
        c_labels.org = gtk_label_new(g_contact.org);
        gtk_label_set_xalign(GTK_LABEL(c_labels.org), 0);
        c_labels.address = gtk_label_new(g_contact.address);
        gtk_label_set_xalign(GTK_LABEL(c_labels.address), 0);
        c_labels.website = gtk_label_new(g_contact.website);
        gtk_label_set_xalign(GTK_LABEL(c_labels.website), 0);
        c_labels.extra = gtk_label_new(g_contact.extra);
        gtk_label_set_xalign(GTK_LABEL(c_labels.extra), 0);

        GtkWidget *action_box;
        GtkWidget *edit_button;
        GtkWidget *delete_button;
        action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        edit_button = gtk_button_new_with_label("Edit");
        delete_button = gtk_button_new_with_label("Delete");
        gtk_container_add(GTK_CONTAINER(action_box), edit_button);
        gtk_container_add(GTK_CONTAINER(action_box), delete_button);
        gtk_container_set_border_width(GTK_CONTAINER(action_box), 5);
        gtk_widget_set_hexpand(edit_button, TRUE);
        gtk_widget_set_hexpand(delete_button, TRUE);
        gtk_widget_set_hexpand(action_box, TRUE);

        g_signal_connect(edit_button, "clicked", G_CALLBACK(switch_to_edit_contact_frame), NULL);
        g_signal_connect(delete_button, "clicked", G_CALLBACK(gui_delete_contact), NULL);
        //                                                        x, y, w, h
        gtk_grid_attach(GTK_GRID(grid), name_label              , 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), title_label             , 1, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), phone_label             , 1, 3, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), email_label             , 1, 4, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), org_label               , 1, 5, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), address_label           , 1, 6, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), website_label           , 1, 7, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), extra_label             , 1, 8, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.name           , 2, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.title          , 2, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.phone          , 2, 3, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.email          , 2, 4, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.org            , 2, 5, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.address        , 2, 6, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.website        , 2, 7, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), c_labels.extra          , 2, 8, 1, 1);
        gtk_grid_attach(GTK_GRID(grid), action_box              , 1, 9, 2, 1);
        gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
        gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
        gtk_container_set_border_width(GTK_CONTAINER(grid), 5);

        gtk_container_add(GTK_CONTAINER(g_view_frame), grid);

        gtk_widget_show_all(g_view_frame);
        alloc_frame_size();
    }
}

static void switch_to_new_contact_frame() {
    alloc_frame_size();
    remove_child_from(g_view_frame);

    g_contact.name = "";
    g_contact.title = "";
    g_contact.phone = "";
    g_contact.email = "";
    g_contact.org = "";
    g_contact.address = "";
    g_contact.website = "";
    g_contact.extra = "";
    g_contact.photoloc = "";

    GtkWidget *new_contact_grid;
        GtkWidget *name_label;
        GtkWidget *title_label;
        GtkWidget *phone_label;
        GtkWidget *email_label;
        GtkWidget *org_label;
        GtkWidget *address_label;
        GtkWidget *website_label;
        GtkWidget *extra_label;
        GtkWidget *photoloc_label;
        GtkWidget *save_button;

    g_entries.name     = gtk_entry_new();
    name_label      = gtk_label_new("Name: ");
    gtk_label_set_xalign(GTK_LABEL(name_label), 1);
    gtk_widget_set_hexpand(g_entries.name, TRUE);

    g_entries.title    = gtk_entry_new();
    title_label     = gtk_label_new("Title: ");
    gtk_label_set_xalign(GTK_LABEL(title_label), 1);
    gtk_widget_set_hexpand(g_entries.title, TRUE);

    g_entries.phone    = gtk_entry_new();
    phone_label     = gtk_label_new("Phone Number: ");
    gtk_label_set_xalign(GTK_LABEL(phone_label), 1);
    gtk_widget_set_hexpand(g_entries.phone, TRUE);
    
    g_entries.email    = gtk_entry_new();
    email_label     = gtk_label_new("Email: ");
    gtk_label_set_xalign(GTK_LABEL(email_label), 1);
    gtk_widget_set_hexpand(g_entries.email, TRUE);

    g_entries.org      = gtk_entry_new();
    org_label       = gtk_label_new("Organization: ");
    gtk_label_set_xalign(GTK_LABEL(org_label), 1);
    gtk_widget_set_hexpand(g_entries.org, TRUE);
    
    g_entries.address  = gtk_entry_new();
    address_label   = gtk_label_new("Address: ");
    gtk_label_set_xalign(GTK_LABEL(address_label), 1);
    gtk_widget_set_hexpand(g_entries.address, TRUE);

    g_entries.website = gtk_entry_new();
    website_label   = gtk_label_new("Website: ");
    gtk_label_set_xalign(GTK_LABEL(website_label), 1);
    gtk_widget_set_hexpand(g_entries.website, TRUE);

    g_entries.extra    = gtk_entry_new();
    extra_label     = gtk_label_new("Extra Info: ");
    gtk_label_set_xalign(GTK_LABEL(extra_label), 1);
    gtk_widget_set_hexpand(g_entries.extra, TRUE);
    gtk_widget_set_vexpand(g_entries.extra, TRUE);

    g_entries.photoloc = gtk_file_chooser_button_new("Photo", GTK_FILE_CHOOSER_ACTION_OPEN);
    photoloc_label  = gtk_label_new("Photo: ");
    gtk_label_set_xalign(GTK_LABEL(photoloc_label), 1);
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(g_entries.photoloc), g_get_home_dir());
    g_signal_connect(g_entries.photoloc, "file-set", G_CALLBACK(on_file_select), &g_contact.photoloc);
    gtk_widget_set_hexpand(g_entries.photoloc, TRUE);

    save_button = gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(gui_save_contact), NULL);
    gtk_widget_set_hexpand(save_button, TRUE);

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
    gtk_grid_attach(GTK_GRID(new_contact_grid), website_label,      0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.website,  1, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), extra_label,        0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.extra,    1, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), photoloc_label,     0, 9, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), g_entries.photoloc, 1, 9, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), save_button,        0,10, 2, 1);
    gtk_grid_set_column_spacing(GTK_GRID(new_contact_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(new_contact_grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(new_contact_grid), 5);

    gtk_frame_set_label(GTK_FRAME(g_view_frame), "New Contact");
    gtk_container_add(GTK_CONTAINER(g_view_frame), new_contact_grid);
    gtk_widget_show_all(g_view_frame);
}

static void open_open_dialog() {
    GtkWidget *dialog;
        GtkFileChooser *chooser;
        GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
        int res;

    dialog = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(g_win), action, ("_Cancel"),
                                         GTK_RESPONSE_CANCEL, ("_Open"), GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);
    gtk_file_chooser_set_current_folder(chooser, g_get_home_dir());

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(chooser);
        if (db_set_open_file(filename) == 1) {
            gui_send_error("File must end in .db");
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void open_backup_dialog() {
    GtkWidget *dialog;
        GtkFileChooser *chooser;
        GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
        int res;

    dialog = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(g_win), action, ("_Cancel"),
                                         GTK_RESPONSE_CANCEL, ("_Save"), GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);
    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
    gtk_file_chooser_set_current_folder(chooser, g_get_home_dir());

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(chooser);
        if (db_backup_at(filename) == 1) {
            gui_send_error("File must end in .db");
        }
        else if (db_backup_at(filename) == -1) {
            gui_send_error("File couldn't be opened");
        }
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void setup_title_bar(GtkWidget *main_grid) {
    GtkWidget *menu_bar = gtk_menu_bar_new();
    GtkWidget *file_item = gtk_menu_item_new_with_label("File");
    GtkWidget *edit_item = gtk_menu_item_new_with_label("Edit");
    GtkWidget *help_item = gtk_menu_item_new_with_label("Help");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_item);

    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget *backup_item = gtk_menu_item_new_with_label("Backup Contacts");
    GtkWidget *open_item = gtk_menu_item_new_with_label("Open Contacts");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), backup_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), open_item);

    GtkWidget *edit_menu = gtk_menu_new();
    GtkWidget *new_contact_item = gtk_menu_item_new_with_label("New Contact");
    GtkWidget *edit_contact_item = gtk_menu_item_new_with_label("Edit Contact");
    GtkWidget *delete_contact_item = gtk_menu_item_new_with_label("Delete Contact");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), new_contact_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_contact_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), delete_contact_item);

    gtk_widget_set_hexpand(GTK_WIDGET(menu_bar), TRUE);
    gtk_grid_attach(GTK_GRID(main_grid), menu_bar, 0, 0, 2, 1);

    g_signal_connect(backup_item, "activate", G_CALLBACK(open_backup_dialog), NULL);
    g_signal_connect(open_item, "activate", G_CALLBACK(open_open_dialog), NULL);
    g_signal_connect(new_contact_item, "activate", G_CALLBACK(switch_to_new_contact_frame), NULL);
    g_signal_connect(edit_contact_item, "activate", G_CALLBACK(switch_to_edit_contact_frame), NULL);
    g_signal_connect(delete_contact_item, "activate", G_CALLBACK(gui_delete_contact), NULL);
    g_signal_connect(help_item, "activate", G_CALLBACK(help_open_window), NULL);
}

static void setup_action_box(GtkWidget *main_grid) {
    GtkWidget *action_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *new_contact_button = gtk_button_new_with_label("New Contact");
    GtkWidget *edit_contact_button = gtk_button_new_with_label("Edit Contact");
    GtkWidget *delete_contact_button = gtk_button_new_with_label("Delete Contact");
    gtk_box_pack_start(GTK_BOX(action_box), new_contact_button, FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(action_box), edit_contact_button, FALSE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(action_box), delete_contact_button, FALSE, TRUE, 5);

    gtk_grid_attach(GTK_GRID(main_grid), action_box, 0, 1, 2, 1);
    gtk_container_set_border_width(GTK_CONTAINER(action_box), 5);

    g_signal_connect(new_contact_button, "clicked", G_CALLBACK(switch_to_new_contact_frame), NULL);
    g_signal_connect(edit_contact_button, "clicked", G_CALLBACK(switch_to_edit_contact_frame), NULL);
    g_signal_connect(delete_contact_button, "clicked", G_CALLBACK(gui_delete_contact), NULL);
}
 
static void setup_list_frame(GtkWidget *main_grid) {
    g_list_view.view = list_create_view();
    GtkWidget *list_scroller = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(list_scroller), g_list_view.view);
    gtk_widget_set_hexpand(list_scroller, TRUE);
    gtk_widget_set_vexpand(list_scroller, TRUE);

    GtkWidget *list_grid = gtk_grid_new();
    GtkWidget *search_entry = gtk_search_entry_new();
    GtkWidget *refresh_button = gtk_button_new_from_icon_name("view-refresh-symbolic",
            GTK_ICON_SIZE_BUTTON);
    GtkWidget *search_opts_box = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(search_opts_box), "Name");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(search_opts_box), "Title");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(search_opts_box), "Phone Number");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(search_opts_box), "Email");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(search_opts_box), "Org");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(search_opts_box), "Address");
    gtk_widget_set_hexpand(search_entry, TRUE);
    gtk_grid_attach(GTK_GRID(list_grid), search_entry, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(list_grid), search_opts_box, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(list_grid), refresh_button, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(list_grid), list_scroller, 0, 1, 3, 2);
    gtk_grid_set_column_spacing(GTK_GRID(list_grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(list_grid), 5);

    GtkWidget *list_frame = gtk_frame_new("List");
    gtk_container_add(GTK_CONTAINER(list_frame), list_grid);

    gtk_widget_set_hexpand(list_frame, TRUE);
    gtk_widget_set_vexpand(list_frame, TRUE);
    gtk_frame_set_label_align(GTK_FRAME(list_frame), 0.5, 0);
    gtk_grid_attach(GTK_GRID(main_grid), list_frame, 0, 2, 1, 1);

    g_signal_connect(search_entry, "search-changed", G_CALLBACK(search_callback), NULL);
    g_signal_connect(search_opts_box, "changed", G_CALLBACK(set_search_col), NULL);
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(list_refresh), NULL);

    GtkTreeSelection *list_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(g_list_view.view));
    g_signal_connect(list_selection, "changed", G_CALLBACK(switch_to_view_contact_frame), NULL);

    gtk_frame_set_shadow_type(GTK_FRAME(list_frame), GTK_SHADOW_OUT);
    gtk_container_set_border_width(GTK_CONTAINER(list_frame), 5);
    //This has to be all the way out here because if it's not, the search_opt
    //value is not set and there's a segfault
    gtk_combo_box_set_active(GTK_COMBO_BOX(search_opts_box), 0);
}

static void setup_view_frame(GtkWidget *main_grid) {
    g_view_frame = gtk_frame_new("View");
    gtk_frame_set_label_align(GTK_FRAME(g_view_frame), 0.5, 0);
    gtk_widget_set_hexpand(g_view_frame, FALSE);
    gtk_widget_set_vexpand(g_view_frame, TRUE);
    gtk_grid_attach(GTK_GRID(main_grid), g_view_frame, 1, 2, 1, 1);
    gtk_frame_set_shadow_type(GTK_FRAME(g_view_frame), GTK_SHADOW_OUT);
    gtk_container_set_border_width(GTK_CONTAINER(g_view_frame), 5);
}

static void setup_main_window(GtkApplication *app) {
    GtkWidget *main_grid = gtk_grid_new();

    setup_title_bar(main_grid);
    setup_action_box(main_grid);
    setup_list_frame(main_grid);
    setup_view_frame(main_grid);

    //Make window
    g_win = gtk_application_window_new(app);
    gtk_container_add(GTK_CONTAINER(g_win), main_grid);
    gtk_window_set_default_size(GTK_WINDOW(g_win), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_show_all(g_win);

    alloc_frame_size();
}

int gui_init(int argc, char **argv) {
    gtk_init(&argc, &argv);

    GtkApplication *app;
    int status;
    app = gtk_application_new("com.com.com", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(setup_main_window), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
