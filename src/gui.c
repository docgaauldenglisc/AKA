#include <gtk/gtk.h>

#include "gui.h"
#include "conoptions.h"

// New Contact Thing
static void take_user_input(GtkWidget *entry, gpointer contact_data) {
    
    EntryWidgets *user_in = (EntryWidgets*) contact_data;
    contact_chars chars;

    chars.name      = gtk_entry_get_text(GTK_ENTRY(user_in->name_entry));
    chars.number    = gtk_entry_get_text(GTK_ENTRY(user_in->number_entry));
    chars.org       = gtk_entry_get_text(GTK_ENTRY(user_in->org_entry));
    chars.email     = gtk_entry_get_text(GTK_ENTRY(user_in->email_entry));
    chars.address   = gtk_entry_get_text(GTK_ENTRY(user_in->address_entry));
    
    if ((strcmp(chars.name, "")) == 0) {
        printf("Add a name\n");
    }
    else {
        write_to_file(chars); 
    }

}

static void contact_window(GtkApplication *app, gpointer data) {
    
    GtkWidget *new_contact_window;
        GtkWidget *new_contact_grid;
            GtkWidget *enter_button;

    contact clabels;
    EntryWidgets *entries = g_malloc(sizeof(EntryWidgets));
    entries->name_entry = NULL;
    entries->number_entry = NULL;

    //Set up new_contact_window
    new_contact_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(new_contact_window), "AKA - New Contact");
    gtk_window_set_default_size(GTK_WINDOW(new_contact_window), 500, 500);

    //Set up new_contact_grid
    new_contact_grid = gtk_grid_new();

    //Set up text and entry widgets
    clabels.name_label      = gtk_label_new("Name");
    entries->name_entry     = gtk_entry_new();
    clabels.number_label    = gtk_label_new("Number");
    entries->number_entry   = gtk_entry_new();
    clabels.email_label     = gtk_label_new("Email");
    entries->email_entry    = gtk_entry_new();
    clabels.org_label	    = gtk_label_new("Organization");
    entries->org_entry      = gtk_entry_new();
    clabels.address_label   = gtk_label_new("Address");
    entries->address_entry  = gtk_entry_new();

    //Set up enter_button
    enter_button = gtk_button_new_with_label("Save");
    g_signal_connect(enter_button, "clicked", G_CALLBACK(take_user_input), entries); 
    
    //Attach everything to the grid
    //                                                                  x, y, w, h
    gtk_grid_attach(GTK_GRID(new_contact_grid), clabels.name_label,     1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), entries->name_entry,    1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), clabels.number_label,   2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), entries->number_entry,  2, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), clabels.email_label,	3, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), entries->email_entry,	3, 2, 1, 1);
    
    gtk_grid_attach(GTK_GRID(new_contact_grid), clabels.org_label,	    1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), entries->org_entry,     1, 4, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), clabels.address_label,  2, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(new_contact_grid), entries->address_entry, 2, 4, 1, 1);

    gtk_grid_attach(GTK_GRID(new_contact_grid), enter_button,           4, 5, 1, 1);

    //Present window
    gtk_container_add(GTK_CONTAINER(new_contact_window), new_contact_grid);
    gtk_widget_show_all(new_contact_window);

}

// Main Window
static GtkTreeModel* create_model() {
    
        GtkTreePath *path;
            GtkTreeIter iter;

    int max = get_id(1);

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

static GtkWidget* create_view() {
    
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

static void main_window(GtkApplication *app, gpointer data) {
    
    GtkWidget *main_window;
            GtkWidget *main_grid;
                GtkWidget *top_box;
                    GtkWidget *new_contact_button; 
                    GtkWidget *refresh_button; 
                        GtkWidget *refresh_icon;
                    GtkWidget *contact_search_entry;
                GtkWidget *list_scroll;
                    GtkWidget *bottom_box;
                        GtkWidget *view;
                GtkWidget *scrolled_contact;
                    GtkWidget *side_box;

    main_window             = gtk_application_window_new(app);
    list_scroll             = gtk_scrolled_window_new(NULL, NULL);
    main_grid               = gtk_grid_new();
    top_box                 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    new_contact_button      = gtk_button_new_with_label("+");
    contact_search_entry    = gtk_search_entry_new();
    refresh_button          = gtk_button_new();
    refresh_icon            = gtk_image_new_from_icon_name("view-refresh-symbolic", GTK_ICON_SIZE_BUTTON);
    bottom_box              = gtk_list_box_new();
    scrolled_contact        = gtk_scrolled_window_new(NULL, NULL);
    view                    = create_view();
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), create_model());

    gtk_window_set_title(GTK_WINDOW(main_window), "AKA");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1280, 720);
    
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(list_scroll), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    g_signal_connect(new_contact_button, "clicked", G_CALLBACK(contact_window), NULL);

    g_signal_connect(refresh_button, "clicked", G_CALLBACK(refresh), view);
    
    gtk_widget_set_hexpand(contact_search_entry, TRUE);

    gtk_button_set_image(GTK_BUTTON(refresh_button), refresh_icon);

    gtk_container_add(GTK_CONTAINER(top_box), new_contact_button);
    gtk_container_add(GTK_CONTAINER(top_box), contact_search_entry);
    gtk_container_add(GTK_CONTAINER(top_box), refresh_button);

    gtk_container_add(GTK_CONTAINER(bottom_box), view);

    gtk_container_add(GTK_CONTAINER(list_scroll), bottom_box);
    gtk_grid_attach(GTK_GRID(main_grid), top_box, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(main_grid), list_scroll, 1, 2, 1, 1);
    gtk_widget_set_vexpand(list_scroll, TRUE);

    gtk_container_add(GTK_CONTAINER(main_window), main_grid);
    gtk_widget_show_all(main_window);
    
}


// AKA
int start_gui(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkApplication *app;
    int status;

    app = gtk_application_new("com.com.com", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(main_window), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
