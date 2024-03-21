#ifndef DATABASE_H
#define DATABASE_H

#include "gui.h"

enum {
    SAVE_CONTACT = 0,
    EDIT_CONTACT
};

void db_save_contact(Contact *con, int mode);
char *db_get(char *col, int row);
char *db_get_table_name();
int db_max_id();
int db_init();

#endif
