#ifndef CONOPTIONS_H
#define CONOPTIONS_H

typedef struct contact_widgets {

    GtkWidget *name_label;
    GtkWidget *number_label;

} contact; 

typedef struct contact_entry_widgets {
    
    GtkWidget *name_entry;
    GtkWidget *number_entry;

} entries;

typedef struct user_input{
    
     char *name;
     char *number;

} contact_chars;

#endif
