#ifndef CONOPTIONS_H
#define CONOPTIONS_H

typedef struct contact_widgets {

    GtkWidget *name_label;
    GtkWidget *number_label;
    GtkWidget *email_label;
    GtkWidget *org_label;
    GtkWidget *address_label;

} contact; 

typedef struct contact_entry_widgets {
    
    GtkWidget *name_entry;
    GtkWidget *number_entry;
    GtkWidget *email_entry;
    GtkWidget *org_entry;
    GtkWidget *address_entry;

} entries;

typedef struct user_input{
    
     char *name;
     char *number;
     char *email;
     char *org;
     char *address;

} contact_chars;

#endif
