
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
