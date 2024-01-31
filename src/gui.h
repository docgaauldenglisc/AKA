#ifndef GUI_H
#define GUI_H

static void take_user_input();
static void new_contact_frame();
static GtkTreeModel *create_model();
static GtkWidget *create_view();
static void refresh();
static void change_view_frame_size();
static void main_window(GtkApplication *app);
int start_gui(int argc, char **argv);

#endif
