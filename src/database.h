#ifndef DATABASE_H
#define DATABASE_H

typedef struct {
    int *ids;
    int id_amount;
} idList;

void db_save_contact(ContactText *con);
void db_edit_contact(ContactText *con);
idList db_search(char *query);
char *db_get(char *col, int row);
int db_max_id();
int db_init();

#endif
