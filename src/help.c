#include <gtk/gtk.h>

#include "help.h"
#include "gui.h"

void help_open_window();

void help_open_window() {
    GtkWidget *win;
        GtkWidget *help_box;
            GtkWidget *scrolled_text_container;
                GtkWidget *text_view;
                    GtkTextBuffer *text_buf;
                    GtkTextIter iter;

    char *guide = NULL;
    FILE *file;
    size_t len = 0;
    file = fopen("../src/help/faq/creatingacontact", "r");
    if (file == NULL) {
        puts("Can't open file!");
        return;
    }
    getdelim(&guide, &len, '\0', file);

    win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    help_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    scrolled_text_container = gtk_scrolled_window_new(NULL, NULL);
    text_view = gtk_text_view_new();

    text_buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    gtk_text_buffer_get_end_iter(text_buf, &iter);

    gtk_text_buffer_insert_markup(text_buf, &iter, guide, strlen(guide)); 

    gtk_widget_set_hexpand(text_view, TRUE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);

    gtk_container_add(GTK_CONTAINER(scrolled_text_container), text_view);
    gtk_container_add(GTK_CONTAINER(help_box), scrolled_text_container);

    gtk_container_add(GTK_CONTAINER(win), help_box);
    gtk_window_set_default_size(GTK_WINDOW(win), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_show_all(win);
}
