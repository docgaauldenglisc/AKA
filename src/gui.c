#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>

#include "conoptions.h"
#include "database.h"

#define WINDOW_WIDTH 720
#define WINDOW_HEIGHT 720

EntryWidgets entries;
contact_chars chars;
contact clabels;

GtkWidget *window;
    GtkWidget *main_frame;
        GtkWidget *main_grid;
            GtkWidget *top_box;
            GtkWidget *o_box;
                GtkWidget *new_contact_button;
                GtkWidget *view_frame;
                //New Contact Frame
                    GtkWidget *new_contact_grid;
                        GtkWidget *enter_button;
                GtkWidget *list_grid;
                    GtkWidget *scroll_list;
                        GtkWidget *list_box;
                            GtkWidget *search_entry;
                            GtkWidget *refresh_button;
                                GtkWidget *refresh_icon;
                                    GtkWidget *view;

enum {
    COL_ID = 0,
    COL_NAME,
    COL_NUMBER,
    COL_EMAIL,
    COL_ORG,
    COL_ADDRESS,
    NUM_COLS
};

static void take_user_input() {
    chars.name      = gtk_entry_get_text(GTK_ENTRY(entries.name_entry));
    chars.number    = gtk_entry_get_text(GTK_ENTRY(entries.number_entry));
    chars.org       = gtk_entry_get_text(GTK_ENTRY(entries.org_entry));
    chars.email     = gtk_entry_get_text(GTK_ENTRY(entries.email_entry));
    chars.address   = gtk_entry_get_text(GTK_ENTRY(entries.address_entry));
    
    if ((strcmp(chars.name, "")) == 0) {
        printf("Add a name\n");
    }
    else {
        write_to_file(chars); 
    }
}

static void new_contact_frame() {
    gtk_frame_set_label(GTK_FRAME(view_frame), "New Contact");

    new_contact_grid = gtk_grid_new();

        //Set up text and entry widgets
    clabels.name_label      = gtk_label_new("Name");
    entries.name_entry     = gtk_entry_new();
    clabels.number_label    = gtk_label_new("Number");
    entries.number_entry   = gtk_entry_new();
    clabels.email_label     = gtk_label_new("Email");
    entries.email_entry    = gtk_entry_new();
    clabels.org_label	    = gtk_label_new("Organization");
    entries.org_entry      = gtk_entry_new();
    clabels.address_label   = gtk_label_new("Address");
    entries.address_entry  = gtk_entry_new();

    //Set up enter_button
    enter_button = gtk_button_new_with_label("Save");
    g_signal_connect(enter_button, "clicked", G_CALLBACK(take_user_input), NULL); 
    
    //Attach everything to the grid
    //                                                                  x, y, w, h
    gtk_grid_attach(GTK_GRID(new_contact_grid), clabels.name_label,     1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), entries.name_entry,     1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), clabels.number_label,   2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), entries.number_entry,   2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), clabels.email_label,	3, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), entries.email_entry,	3, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), clabels.org_label,	    1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), entries.org_entry,      1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), clabels.address_label,  2, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), entries.address_entry,  2, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), enter_button,           4, 5, 1, 1);

    //Present into frame 
    gtk_container_add(GTK_CONTAINER(view_frame), new_contact_grid);
    gtk_widget_show_all(view_frame);
}

static GtkTreeModel *create_model() {
    GtkTreeIter iter;

    int max = get_id();

    GtkListStore *store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    for (int i = 0; i < max; i++) {
        char id[50];
        char name[50];
        char number[50];
        char email[50];
        char org[50];
        char address[50];

        get_from_col_and_row("ID"    , (i + 1), id);
        get_from_col_and_row("NAME"    , (i + 1), name);
        get_from_col_and_row("NUMBER"  , (i + 1), number);
        get_from_col_and_row("EMAIL"   , (i + 1), email);
        get_from_col_and_row("ORG"     , (i + 1), org);
        get_from_col_and_row("ADDRESS" , (i + 1), address);

        printf("%i Gotten name of: %s\n", (i + 1), name);
    
        gtk_list_store_append(GTK_LIST_STORE(store), &iter);
        gtk_list_store_set(store, &iter, COL_ID, id, COL_NAME, name, COL_NUMBER, number, COL_EMAIL, email, COL_ORG, org, COL_ADDRESS, address, -1);
    }
    return GTK_TREE_MODEL(store);
}

static GtkWidget *create_view() {
   GtkCellRenderer *renderer;
    GtkWidget *view = gtk_tree_view_new();

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "ID"       , renderer, "text", COL_ID, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Name"     , renderer, "text", COL_NAME, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Number"   , renderer, "text", COL_NUMBER, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Email"    , renderer, "text", COL_EMAIL, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Org"      , renderer, "text", COL_ORG, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Address"  , renderer, "text", COL_ADDRESS, NULL);
    
    GtkTreeModel *model = create_model();
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

    g_object_unref(model);

    return view;
}

static void refresh(GtkWidget *not_used, GtkTreeView *view) {
    GtkTreeModel *model = create_model();

    if (gtk_tree_view_get_model(view) != NULL) {
        g_object_unref(gtk_tree_view_get_model(view));
    }

    gtk_tree_view_set_model(view, model);
}

static void change_view_frame_size() {
    GtkAllocation allocation;
    gtk_widget_get_allocation(window, &allocation);

    int min_width = (int)(0.55 * allocation.width);    

    gtk_widget_set_size_request(GTK_WIDGET(view_frame), min_width, -1);
}

static void main_window(GtkApplication *app) {
    window = gtk_application_window_new(app);
    main_grid = gtk_grid_new();
    main_frame = gtk_frame_new("AKA");
    top_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    o_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    new_contact_button = gtk_button_new_with_label("+");
    view_frame = gtk_frame_new(NULL);
    list_grid = gtk_grid_new();
    search_entry = gtk_search_entry_new();
    refresh_button = gtk_button_new();
    refresh_icon = gtk_image_new_from_icon_name("view-refresh-symbolic", GTK_ICON_SIZE_BUTTON);
    scroll_list = gtk_scrolled_window_new(NULL, NULL);
    list_box = gtk_list_box_new();
    view = create_view();

    gtk_widget_set_hexpand(new_contact_button, FALSE);
    gtk_box_pack_start(GTK_BOX(o_box), new_contact_button, FALSE, FALSE, 0); 

    g_signal_connect(new_contact_button, "clicked", G_CALLBACK(new_contact_frame), NULL);
    // A little complicated, but it should be fine.
    // The view frame should be 55% of the width of the window at all times.
    g_signal_connect(window, "size-allocate", G_CALLBACK(change_view_frame_size), view_frame);

    gtk_button_set_image(GTK_BUTTON(refresh_button), refresh_icon);
    g_signal_connect(refresh_button, "clicked", G_CALLBACK(refresh), view);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_list), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_vexpand(scroll_list, TRUE);
    gtk_widget_set_hexpand(refresh_button, FALSE);
    gtk_widget_set_hexpand(search_entry, TRUE);

    gtk_tree_view_set_model(GTK_TREE_VIEW(view), create_model());

    gtk_grid_attach(GTK_GRID(list_grid), search_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(list_grid), refresh_button, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(list_grid), scroll_list, 1, 2, 2, 2);
    gtk_container_add(GTK_CONTAINER(scroll_list), list_box);
    gtk_container_add(GTK_CONTAINER(list_box), view);

  //gtk_grid_attach(grid,              , child      , x, y, w, h);
    gtk_grid_attach(GTK_GRID(main_grid), top_box    , 1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(main_grid), o_box      , 1, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(main_grid), view_frame , 2, 3, 1, 2);
    gtk_grid_attach(GTK_GRID(main_grid), list_grid  , 1, 3, 1, 2);

    gtk_frame_set_label_align(GTK_FRAME(view_frame), 0.5, 1.0);
    gtk_frame_set_label_align(GTK_FRAME(main_frame), 0.5, 1.0);
    gtk_container_add(GTK_CONTAINER(main_frame), main_grid);
    gtk_container_add(GTK_CONTAINER(window), main_frame);

    gtk_window_set_title(GTK_WINDOW(window), "AKA");
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_show_all(window);
}

int start_gui(int argc, char **argv) {
    GtkApplication *app;
    int status;

    gtk_init(&argc, &argv);

    app = gtk_application_new("com.com.com", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(main_window), NULL); 
    status  = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    printf("%i\n", status);
    return status;
}
