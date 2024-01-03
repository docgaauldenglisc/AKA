
static void mainWindow (GtkApplication *app, // Main window
                      gpointer        user_data) {
    
    //Declaration of widgets
    GtkWidget *mainWindow;
        GtkWidget *mainBox;
            GtkWidget *topBox;
                GtkWidget *menuButton;
                GtkWidget *searchText;
            GtkWidget *contactList;
                GtkListItemFactory *factory;

    //Set up window
    mainWindow = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(mainWindow), "Window");
    gtk_window_set_default_size(GTK_WINDOW(mainWindow), 750, 750);

    //Set up mainBox
    mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    
    //Set up topBox
    topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

    //Set up menuButton    
    menuButton = gtk_button_new_with_label("+");
    gtk_widget_set_halign(menuButton, GTK_ALIGN_START);

    g_signal_connect(menuButton, "clicked", G_CALLBACK(newContactWindow), NULL);

    //Set up searchBar
    searchText = gtk_search_entry_new();

    gtk_widget_set_hexpand(searchText, TRUE);

    //Set up contactList
    factory = gtk_signal_list_item_factory_new();
    contactList = gtk_list_view_new(NULL, factory);


    //Ready topBox
    gtk_box_append(GTK_BOX(topBox), menuButton);
    gtk_box_append(GTK_BOX(topBox), searchText);

    //Ready mainBox
    gtk_window_set_child(GTK_WINDOW(mainWindow), mainBox);
    gtk_box_append(GTK_BOX(mainBox), topBox);
    gtk_box_append(GTK_BOX(mainBox), contactList);

    //Present window
    gtk_window_present(GTK_WINDOW(mainWindow));
}
