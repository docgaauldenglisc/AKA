#include <gtk/gtk.h>

#include "help.h"
#include "gui.h"

GtkWidget *g_text_view;
GtkWidget *g_guide_frame;

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
};

static GtkTreeModel *list_create_model() {
    GtkTreeIter parent_iter;
    GtkTreeIter child_iter;
    GtkTreeStore *store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);

    gtk_tree_store_append(store, &parent_iter, NULL);
    gtk_tree_store_set(store, &parent_iter, COL_SECTION, "Actions", COL_GUIDE, "", COL_FILENAME, ACTIONS, -1);

    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Creating a Contact", COL_FILENAME, CREATINGACONTACT, -1);

    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Deleting a Contact", COL_FILENAME, DELETINGACONTACT, -1);

    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Editing a Contact", COL_FILENAME, EDITINGACONTACT, -1);

    gtk_tree_store_append(store, &parent_iter, NULL);
    gtk_tree_store_set(store, &parent_iter, COL_SECTION, "Contact Information", COL_GUIDE, "", COL_FILENAME, CONTACTINFORMATION, -1);

    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Name", COL_FILENAME, NAMEEXPLAINED, -1);

    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Title", COL_FILENAME, TITLEEXPLAINED, -1);

    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Phone Number", COL_FILENAME, PHONENUMBEREXPLAINED, -1);

    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Email", COL_FILENAME, EMAILEXPLAINED, -1);

    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Organization", COL_FILENAME, ORGEXPLAINED, -1);

    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "Address", COL_FILENAME, ADDRESSEXPLAINED, -1);

    GtkTreeModel *model = GTK_TREE_MODEL(store);
    return model;
}

static GtkWidget *list_create_view() {
    GtkCellRenderer *renderer;
    GtkTreeModel *model = list_create_model();
    GtkWidget *view = gtk_tree_view_new();

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Section", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Guide", renderer, "text", 1, NULL);

    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

    return view;
}

static void text_create_buffer() {
    //g_text_view;    
    GtkTextBuffer *buf = gtk_text_buffer_new(NULL);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(g_text_view), buf);
}

static void open_guide(char *guide) {
    GtkTextBuffer *text_buf;
    GtkTextIter iter;

    text_create_buffer();

    char *article = NULL;
    FILE *file;
    size_t len = 0;
    file = fopen(guide, "r");
    if (file == NULL) {
        puts("Can't open file!");
        return;
    }
    getdelim(&article, &len, '\0', file);

    text_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_text_view));

    gtk_text_buffer_get_end_iter(text_buf, &iter);

    gtk_text_buffer_insert_markup(text_buf, &iter, article, strlen(article)); 

    gtk_widget_set_hexpand(g_text_view, TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(g_text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(g_text_view), FALSE);
}

static void change_guide_to_selection(GtkWidget *selection, gpointer nu) {
    GtkTreeModel *model;
    GtkTreeIter iter;
    int file_num;
    if(gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter)) {
        gtk_tree_model_get(model, &iter, COL_FILENAME, &file_num, -1);
        switch (file_num) {
        case ACTIONS:
            open_guide("../src/help/actions");
            break;
        case CREATINGACONTACT:
            open_guide("../src/help/creatingacontact");
            break;
        case DELETINGACONTACT:
            open_guide("../src/help/deletingacontact");
            break;
        case EDITINGACONTACT:
            open_guide("../src/help/editingacontact");
            break;
        case CONTACTINFORMATION:
            open_guide("../src/help/contactinformation");
            break;
        case NAMEEXPLAINED:
            open_guide("../src/help/nameexplained");
            break;
        case TITLEEXPLAINED:
            open_guide("../src/help/titleexplained");
            break;
        case PHONENUMBEREXPLAINED:
            open_guide("../src/help/phonenumberexplained");
            break;
        case EMAILEXPLAINED:
            open_guide("../src/help/emailexplained");
            break;
        case ORGEXPLAINED:
            open_guide("../src/help/orgexplained");
            break;
        case ADDRESSEXPLAINED:
            open_guide("../src/help/addressexplained");
            break;
        }
    }
}

void help_open_window() {
    GtkWidget *win;
        GtkWidget *help_box;
            GtkWidget *scrolled_list_container;
                GtkWidget *list_view;
                GtkTreeSelection *selection;
            //g_guide_frame;
                GtkWidget *scrolled_text_container;
                //g_text_view;

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    help_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    scrolled_list_container = gtk_scrolled_window_new(NULL, NULL);
    list_view = list_create_view();
    g_guide_frame = gtk_frame_new("");
    scrolled_text_container = gtk_scrolled_window_new(NULL, NULL);
    g_text_view = gtk_text_view_new();

    gtk_widget_set_hexpand(list_view, TRUE);

    gtk_frame_set_label_align(GTK_FRAME(g_guide_frame), 0.5, 1.0);

    gtk_widget_set_hexpand(g_text_view, TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(g_text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(g_text_view), FALSE);

    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_view));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(selection, "changed", G_CALLBACK(change_guide_to_selection), NULL);

    gtk_tree_view_expand_all(GTK_TREE_VIEW(list_view));

    gtk_container_add(GTK_CONTAINER(scrolled_list_container), list_view);
    gtk_container_add(GTK_CONTAINER(help_box), scrolled_list_container);

    gtk_container_add(GTK_CONTAINER(g_guide_frame), scrolled_text_container);
    gtk_container_add(GTK_CONTAINER(scrolled_text_container), g_text_view);

    gtk_container_add(GTK_CONTAINER(help_box), g_guide_frame);
    gtk_container_add(GTK_CONTAINER(win), help_box);

    gtk_window_set_default_size(GTK_WINDOW(win), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_show_all(win);

    open_guide("../src/help/actions");
}
