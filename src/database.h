#ifndef DATABASE_H
#define DATABASE_H

#include "gui.h"

void db_save_contact(Contact *con);
char *db_get(char *col, int row);
int db_max_id();
int db_init();

#endif
