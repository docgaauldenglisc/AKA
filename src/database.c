#include <stdio.h>
#include <sqlite3.h>

#include "database.h"

int database_init() {
    sqlite3 *db;
    int rc;

    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        printf("Database couldn't open!\n%s\n", sqlite3_errmsg(db));
        return rc;
    }

    const char *database_schema = "CREATE TABLE IF NOT EXISTS contacts (" \
                                  "ID INT PRIMARY KEY NOT NULL," \ 
                                  "NAME TEXT NOT NULL," \
                                  "NUMBER TEXT, " \
                                  "EMAIL TEXT, " \ 
                                  "ORG TEXT, " \
                                  "ADDRESS TEXT, " \
                                  "PHOTOLOC TEXT, " \
                                  ");";
}
