#include <gtk/gtk.h>

#include "help.h"
#include "gui.h"

GtkWidget *g_text_view;
GtkWidget *g_guide_frame;

static GtkTreeModel *list_create_model();
static GtkWidget *list_create_view();
static void open_create_contact_guide();
void help_open_window();

enum {
    COL_SECTION = 0,
    COL_GUIDE,
    COL_FILENAME,
    NUM_COLS
};

enum {
    FAQ,
    CREATINGACONTACT
};

static GtkTreeModel *list_create_model() {
    GtkTreeIter parent_iter;
    GtkTreeIter child_iter;
    GtkTreeStore *store = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT);

    gtk_tree_store_append(store, &parent_iter, NULL);
    gtk_tree_store_set(store, &parent_iter, COL_SECTION, "FAQ", COL_GUIDE, "", COL_FILENAME, FAQ, -1);

    gtk_tree_store_append(store, &child_iter, &parent_iter);
    gtk_tree_store_set(store, &child_iter, COL_SECTION, "", COL_GUIDE, "guide", COL_FILENAME, CREATINGACONTACT, -1);

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

static void open_faq_guide() {
    GtkTextBuffer *text_buf;
    GtkTextIter iter;

    char *guide = NULL;
    FILE *file;
    size_t len = 0;
    file = fopen("../src/help/faq", "r");
    if (file == NULL) {
        puts("Can't open file!");
        return;
    }
    getdelim(&guide, &len, '\0', file);

    text_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_text_view));

    gtk_text_buffer_get_end_iter(text_buf, &iter);

    gtk_text_buffer_insert_markup(text_buf, &iter, guide, strlen(guide)); 

    gtk_widget_set_hexpand(g_text_view, TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(g_text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(g_text_view), FALSE);

    gtk_frame_set_label(GTK_FRAME(g_guide_frame), "FAQ");
}

static void open_create_contact_guide() {
    GtkTextBuffer *text_buf;
    GtkTextIter iter;

    char *guide = NULL;
    FILE *file;
    size_t len = 0;
    file = fopen("../src/help/creatingacontact", "r");
    if (file == NULL) {
        puts("Can't open file!");
        return;
    }
    getdelim(&guide, &len, '\0', file);

    text_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_text_view));

    gtk_text_buffer_get_end_iter(text_buf, &iter);

    gtk_text_buffer_insert_markup(text_buf, &iter, guide, strlen(guide)); 

    gtk_widget_set_hexpand(g_text_view, TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(g_text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(g_text_view), FALSE);
}

void help_open_window() {
    GtkWidget *win;
        GtkWidget *help_box;
            GtkWidget *scrolled_list_container;
                GtkWidget *list_view;
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

    gtk_container_add(GTK_CONTAINER(scrolled_list_container), list_view);
    gtk_container_add(GTK_CONTAINER(help_box), scrolled_list_container);

    gtk_container_add(GTK_CONTAINER(g_guide_frame), scrolled_text_container);
    gtk_container_add(GTK_CONTAINER(scrolled_text_container), g_text_view);

    gtk_container_add(GTK_CONTAINER(help_box), g_guide_frame);
    gtk_container_add(GTK_CONTAINER(win), help_box);

    gtk_window_set_default_size(GTK_WINDOW(win), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_show_all(win);

    open_faq_guide();
}
