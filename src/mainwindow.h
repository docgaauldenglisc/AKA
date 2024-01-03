
void list_contacts(GtkWidget *list_box) {

    int maxid = get_max_id();

    for (int i = 0; i < maxid; ++i) {

        GtkWidget *contact_list_row;
            GtkWidget *list_row_grid;
                GtkWidget *contact_name_label;
                GtkWidget *contact_phone_number;
                GtkWidget *empty_space[1];

        contact_list_row = gtk_list_box_row_new();
        list_row_grid = gtk_grid_new();

        char name[50];
        char phone_number[] = "(509) 8675-309\n";

        get_text_from_col("NAME", (i + 1), name);

        fprintf(stdout, "Gotten name of: %s\n", name);

        contact_name_label = gtk_label_new(name);
        contact_phone_number = gtk_label_new(phone_number);
        empty_space[0] = gtk_label_new(" ");

        gtk_widget_set_hexpand(empty_space[0], TRUE);

        //                                                                        x, y, w, h
        gtk_grid_attach(GTK_GRID(list_row_grid), contact_name_label             , 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(list_row_grid), empty_space[0]                 , 2, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(list_row_grid), contact_phone_number           , 3, 1, 1, 1);

        gtk_container_add(GTK_CONTAINER(contact_list_row), list_row_grid);

        gtk_list_box_insert(GTK_LIST_BOX(list_box), contact_list_row, -1);

    }    

}

static void main_window(GtkApplication *app, gpointer data) {
    
    GtkWidget *main_window;
            GtkWidget *main_grid;
                GtkWidget *top_box;
                    GtkWidget *new_contact_button; 
                        GtkWidget *contact_search_entry;
                GtkWidget *scrolled_part;
                    GtkWidget *bottom_box;

    //Set up main_window
    main_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(main_window), "AKA");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1280, 720);
    
    //Set up scrolled_part
    scrolled_part = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_part), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    //Set up main_box
    main_grid = gtk_grid_new(); 

    //Set up top_box
    top_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    
    //Set up new_contact_button
    new_contact_button = gtk_button_new_with_label("+");
    g_signal_connect(new_contact_button, "clicked", G_CALLBACK(contact_window), NULL);
    
    //Set up contact_search_bar
    contact_search_entry = gtk_search_entry_new();
    gtk_widget_set_hexpand(contact_search_entry, TRUE);

    //Add Widgets to top_box
    gtk_container_add(GTK_CONTAINER(top_box), new_contact_button);
    gtk_container_add(GTK_CONTAINER(top_box), contact_search_entry);

    //Set up bottom_box
    bottom_box = gtk_list_box_new();
    
    //Set up the list rows 
   
    list_contacts(bottom_box);

    //Add sub-boxes to main_box
    gtk_container_add(GTK_CONTAINER(scrolled_part), bottom_box);
    //gtk_container_add(GTK_CONTAINER(main_box), top_box);
    gtk_grid_attach(GTK_GRID(main_grid), top_box, 1, 1, 1, 1);
    //gtk_container_add(GTK_CONTAINER(main_box), scrolled_window);
    gtk_grid_attach(GTK_GRID(main_grid), scrolled_part, 1, 2, 1, 1);
    gtk_widget_set_vexpand(scrolled_part, TRUE);

    //Present the stuff
    gtk_container_add(GTK_CONTAINER(main_window), main_grid);
    gtk_widget_show_all(main_window);

}
