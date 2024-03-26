#ifndef DATABASE_H
#define DATABASE_H

#include "gui.h"

void db_save_contact(ContactText *con);
void db_edit_contact(ContactText *con);
char *db_get(char *col, int row);
char *db_get_table_name();
int db_max_id();
int db_init();

#endif
