
typedef struct contactInfo {
    char first[15];
    char last[15];
} Contact;

void writetofile(GtkApplication *app, gpointer user_data) {
    
    const char *make_new_contact =  "INSERT INTO Contacts (FIRSTNAME, LASTNAME) ON DUPLICATE KEY UPDATE" \
                                    "" \

}

static void newContactWindow(GtkApplication *app, // Window for making a new contact
                             gpointer        user_data) {
    GtkWidget *newContactWindow;
        GtkWidget *contactGrid; 
            GtkWidget *firstNameText;
            GtkWidget *firstName;
            GtkWidget *lastName;
            GtkWidget *lastNameText;
            GtkWidget *enterButton;

    //Set up window
    newContactWindow = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(newContactWindow), "New Contact");
    gtk_window_set_default_size(GTK_WINDOW(newContactWindow), 750, 500);

    //Set up box 
    contactGrid = gtk_grid_new(); 

    //Set up contact name entry 
    firstName = gtk_entry_new();
    firstNameText = gtk_label_new("First Name");
    lastName = gtk_entry_new();
    lastNameText = gtk_label_new("Last Name");
    
    //Set up button that saves information
    enterButton = gtk_button_new_with_label("Enter");

    Contact newContact;
    printf(gtk_editable_get_text(GTK_EDITABLE(firstName))); 
    //newContact.last     = gtk_editable_get_text(GTK_EDITABLE(lastName)); 

    g_signal_connect(enterButton, "clicked", NULL, NULL); 

    //Ready contactGrid
    gtk_grid_attach(GTK_GRID(contactGrid), firstNameText, 1, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(contactGrid), firstName, 1, 2, 3, 1);
    gtk_grid_attach(GTK_GRID(contactGrid), lastNameText, 4, 1, 3, 1);
    gtk_grid_attach(GTK_GRID(contactGrid), lastName, 4, 2, 3, 1);
    gtk_grid_attach(GTK_GRID(contactGrid), enterButton, 10, 10, 1, 1); 
    gtk_window_set_child(GTK_WINDOW(newContactWindow), contactGrid);

    //Present window
    gtk_window_present(GTK_WINDOW(newContactWindow));
}

