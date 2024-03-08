#ifndef CONOPTIONS_H
#define CONOPTIONS_H

typedef struct contact_widgets {

    GtkWidget *name_label;
    GtkWidget *number_label;
    GtkWidget *email_label;
    GtkWidget *org_label;
    GtkWidget *address_label;
    GtkWidget *photoloc_label;

} contact_widgets; 

typedef struct contact_entry_widgets {
    
    GtkWidget *name_entry;
    GtkWidget *number_entry;
    GtkWidget *email_entry;
    GtkWidget *org_entry;
    GtkWidget *address_entry;
    GtkWidget *photoloc_entry;

} EntryWidgets;

typedef struct user_input{
    
    char *name;
    char *number;
    char *email;
    char *org;
    char *address;
    char *photoloc;

} contact_chars;


typedef struct {
    int *ids;
    int count;
} idList;

#endif
