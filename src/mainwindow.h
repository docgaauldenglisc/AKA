enum {
    COL_ID = 0,
    COL_NAME,
    COL_NUMBER,
    COL_EMAIL,
    COL_ORG,
    COL_ADDRESS,
    NUM_COLS
};

static GtkTreeModel* create_model() {

        GtkTreePath *path;
            GtkTreeIter iter;

    int max = get_id(1);

<<<<<<< HEAD
    GtkListStore *store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
=======
        GtkWidget *contact_list_row;
            GtkWidget *list_row_grid;
                GtkWidget *contact_name;
                GtkWidget *contact_number;
                GtkWidget *contact_email;
                GtkWidget *contact_org;
                GtkWidget *contact_address;
                GtkWidget *empty_space[4];

        contact_list_row = gtk_list_box_row_new();
        list_row_grid = gtk_grid_new();
>>>>>>> 55c0678cae608418de715457ffc49321edd84b06

    for (int i = 0; i < max; i++) {
        char id[50];
        char name[50];
        char number[50];
        char email[50];
        char org[50];
        char address[50];

<<<<<<< HEAD
        get_from_col_and_row("ID"    , (i + 1), id);
        get_from_col_and_row("NAME"    , (i + 1), name);
        get_from_col_and_row("NUMBER"  , (i + 1), number);
        get_from_col_and_row("EMAIL"   , (i + 1), email);
        get_from_col_and_row("ORG"     , (i + 1), org);
        get_from_col_and_row("ADDRESS" , (i + 1), address);
=======
        get_text_from_col("NAME"    , (i + 1), name);
        get_text_from_col("NUMBER"  , (i + 1), number);
        get_text_from_col("EMAIL"   , (i + 1), email);
        get_text_from_col("ORG"     , (i + 1), org);
        get_text_from_col("ADDRESS" , (i + 1), address);
>>>>>>> 55c0678cae608418de715457ffc49321edd84b06

        printf("%i Gotten name of: %s\n", (i + 1), name);
    
        gtk_list_store_append(GTK_LIST_STORE(store), &iter);
        gtk_list_store_set(store, &iter, COL_ID, id, COL_NAME, name, COL_NUMBER, number, COL_EMAIL, email, COL_ORG, org, COL_ADDRESS, address, -1);

<<<<<<< HEAD
    }
    return GTK_TREE_MODEL(store);

}

static GtkWidget* create_view() {
=======
        contact_name = gtk_label_new(name);
        contact_number = gtk_label_new(number);
        contact_email = gtk_label_new(email);
        contact_org = gtk_label_new(org);
        contact_address = gtk_label_new(address);

        for (int i = 0; i < 4; ++i) { 
            empty_space[i] = gtk_label_new(" ");
            gtk_widget_set_hexpand(empty_space[i], TRUE);
        }
        
        //                                                            x, y, w, h
        gtk_grid_attach(GTK_GRID(list_row_grid), contact_name       , 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(list_row_grid), empty_space[0]     , 2, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(list_row_grid), contact_number     , 3, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(list_row_grid), empty_space[1]     , 4, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(list_row_grid), contact_email      , 5, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(list_row_grid), empty_space[2]     , 6, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(list_row_grid), contact_org        , 7, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(list_row_grid), empty_space[3]     , 8, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(list_row_grid), contact_address    , 9, 1, 1, 1);
>>>>>>> 55c0678cae608418de715457ffc49321edd84b06

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
