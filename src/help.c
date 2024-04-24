//Libraries
#include <gtk/gtk.h>

//Local Files
#include "help.h"
#include "gui.h"

GtkWidget *g_text_view;

static GtkTreeModel *list_create_model();
static GtkWidget *list_create_view();
static void text_create_buffer();
static void open_guide(char *guide);
void help_open_window();

enum {
    COL_SECTION = 0,
    COL_GUIDE,
    COL_FILENAME,
    NUM_COLS
};

enum {
    ACTIONS,
    CREATINGACONTACT,
    DELETINGACONTACT,
    EDITINGACONTACT,
    CONTACTINFORMATION,
    NAMEEXPLAINED,
    TITLEEXPLAINED,
    PHONENUMBEREXPLAINED,
    EMAILEXPLAINED,
    ORGEXPLAINED,
    ADDRESSEXPLAINED,
    WEBSITEEXPLAINED,
    EXTRAEXPLAINED,
    MAKINGABACKUP,
    OPENINGABACKUP,
};

static GtkTreeModel *list_create_model() {
    //Welcome to unreadable city.
    //Basically, it adds each article that should be in the help folder into the list
    GtkTreeIter parent_iter;
    GtkTreeIter child_iter;
    GtkTreeStore *store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);
    gtk_tree_store_append(store, &parent_iter, NULL);
    gtk_tree_store_set(store, &parent_iter, COL_SECTION, "Actions", COL_GUIDE, "", COL_FILENAME,
            ACTIONS, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Creating a Contact",
            COL_FILENAME, CREATINGACONTACT, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Deleting a Contact",
            COL_FILENAME, DELETINGACONTACT, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Editing a Contact",
            COL_FILENAME, EDITINGACONTACT, -1);
    gtk_tree_store_append(store, &parent_iter, NULL);
    gtk_tree_store_set(store, &parent_iter, COL_SECTION, "Contact Information", COL_GUIDE, "",
            COL_FILENAME, CONTACTINFORMATION, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Name", COL_FILENAME,
            NAMEEXPLAINED, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Title", COL_FILENAME,
            TITLEEXPLAINED, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Phone Number",
            COL_FILENAME, PHONENUMBEREXPLAINED, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Email", COL_FILENAME,
            EMAILEXPLAINED, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Organization",
            COL_FILENAME, ORGEXPLAINED, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Address", COL_FILENAME,
            ADDRESSEXPLAINED, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Website", COL_FILENAME,
            WEBSITEEXPLAINED, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Extra", COL_FILENAME,
            EXTRAEXPLAINED, -1);
    gtk_tree_store_append(store, &parent_iter, NULL);
    gtk_tree_store_set(store, &parent_iter, COL_SECTION, "Backups", COL_GUIDE, "",
            COL_FILENAME, CONTACTINFORMATION, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Making a Save", COL_FILENAME,
            MAKINGABACKUP, -1);
    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Opening a Save", COL_FILENAME,
            OPENINGABACKUP, -1);

    GtkTreeModel *model = GTK_TREE_MODEL(store);
    return model;
}

static GtkWidget *list_create_view() {
    GtkTreeModel *model = list_create_model();
    GtkWidget *view = gtk_tree_view_new();
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Section", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Guide", renderer, "text", 1, NULL);

    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

    return view;
}

static void text_create_buffer() {
    GtkTextBuffer *buf = gtk_text_buffer_new(NULL);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(g_text_view), buf);
}

static char *read_file(FILE *file, char *loc) {
    char *article = NULL;
    size_t len = 0;
    file = fopen(loc, "r");
    if (file == NULL) {
        puts("Can't open file!");
        return NULL;
    }
    //gets everything in the file until it reaches NULL, the end of the file
    //puts it into the output parameter "article"
    getdelim(&article, &len, '\0', file);
    return article;
}

static void open_guide(char *guide) {
    GtkTextIter iter;
    FILE *file = NULL;

    char *article = read_file(file, guide);
    text_create_buffer();

    GtkTextBuffer *text_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_text_view));
    gtk_text_buffer_get_end_iter(text_buf, &iter);
    gtk_text_buffer_insert_markup(text_buf, &iter, article, strlen(article)); 

    gtk_widget_set_hexpand(g_text_view, TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(g_text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(g_text_view), FALSE);
}

static void change_guide_to_selection(GtkWidget *selection) {
    GtkTreeModel *model;
    GtkTreeIter iter;
    int file_num;
    if(gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
        gtk_tree_model_get(model, &iter, COL_FILENAME, &file_num, -1);
        switch (file_num) {
        case ACTIONS:
            open_guide("help/actions");
            break;
        case CREATINGACONTACT:
            open_guide("help/creatingacontact");
            break;
        case DELETINGACONTACT:
            open_guide("help/deletingacontact");
            break;
        case EDITINGACONTACT:
            open_guide("help/editingacontact");
            break;
        case CONTACTINFORMATION:
            open_guide("help/contactinformation");
            break;
        case NAMEEXPLAINED:
            open_guide("help/nameexplained");
            break;
        case TITLEEXPLAINED:
            open_guide("help/titleexplained");
            break;
        case PHONENUMBEREXPLAINED:
            open_guide("help/phonenumberexplained");
            break;
        case EMAILEXPLAINED:
            open_guide("help/emailexplained");
            break;
        case ORGEXPLAINED:
            open_guide("help/orgexplained");
            break;
        case ADDRESSEXPLAINED:
            open_guide("help/addressexplained");
            break;
        case WEBSITEEXPLAINED:
            open_guide("help/websiteexplained");
            break;
        case EXTRAEXPLAINED:
            open_guide("help/extraexplained");
            break;
        case MAKINGABACKUP:
            open_guide("help/makingabackup");
            break;
        case OPENINGABACKUP:
            open_guide("help/openingabackup");
            break;
        }
    }
}

void help_open_window() {
    GtkWidget *list_view = list_create_view();
    gtk_widget_set_hexpand(list_view, TRUE);

    //Where article can be seen
    g_text_view = gtk_text_view_new();
    gtk_widget_set_hexpand(g_text_view, TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(g_text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(g_text_view), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(g_text_view), 5);

    //handle selections so I can change the guide
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_view));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(selection, "changed", G_CALLBACK(change_guide_to_selection), NULL);

    gtk_tree_view_expand_all(GTK_TREE_VIEW(list_view));

    GtkWidget *scrolled_list_container = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_list_container), list_view);

    GtkWidget *scrolled_text_container = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_text_container), g_text_view);

    //box for the main window
    GtkWidget *help_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(help_box), scrolled_list_container);
    gtk_container_add(GTK_CONTAINER(help_box), scrolled_text_container);
    gtk_container_set_border_width(GTK_CONTAINER(help_box), 5);

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_add(GTK_CONTAINER(win), help_box);

    gtk_window_set_default_size(GTK_WINDOW(win), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_show_all(win);

    //Open guide that's selected when you open help
    open_guide("help/actions");
}
