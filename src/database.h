#ifndef DATABASE_H
#define DATABASE_H

typedef struct {
    char *id;
    char *name;
    char *title;
    char *phone;
    char *email;
    char *org;
    char *address;
    char *extra;
    char *photoloc;
} ContactText;

typedef struct {
    int *ids;
    int id_amount;
} idList;

void db_backup_at(char *filename);
void db_delete_contact(int id);
int db_save_contact(ContactText *con);
int db_edit_contact(ContactText *con);
idList db_search(char *query);
char *db_get(char *col, int row);
int db_max_id();
int db_init();

#endif
