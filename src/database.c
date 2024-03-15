#include <stdio.h>
#include <sqlite3.h>

#include "gui.h"
#include "database.h"

static int iterator_callback(void *data, int argc, char **argv, char **nu) {
    int *max_id = (int *)data;

    if (argv[0] == NULL) {
        printf("There is no number\n");
        *max_id = 0;
    }
    else {
        *max_id = atoi(argv[0]);
    }

    return 0;
}

int max_id() {
    sqlite3 *db;
    int rc;
    int max_id;
    char *err;

    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        printf("Database couldn't open!\n%s\n", sqlite3_errmsg(db));
        return -1;
    }

    char *get_max_id = "SELECT MAX(ID) FROM Contacts";

    sqlite3_exec(db, get_max_id, iterator_callback, &max_id, &err);
    return max_id;
}

void db_save_contact(Contact *con) {
    sqlite3 *db;
    sqlite3_stmt *make_new_contact;
    int rc;

    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        printf("Database couldn't open!\n%s\n", sqlite3_errmsg(db));
        return;
    }

    char *new_contact_temp = "INSERT INTO Contacts(" \
                             "ID," \
                             "NAME," \
                             "NUMBER," \
                             "EMAIL," \
                             "ORG," \
                             "ADDRESS," \
                             "PHOTOLOC) " \
                             "VALUES(" \
                             "?," \
                             "?," \
                             "?," \
                             "?," \
                             "?," \
                             "?," \
                             "?);";
    if (sqlite3_prepare_v2(db, new_contact_temp, -1, &make_new_contact, NULL) != SQLITE_OK) {
        fprintf(stderr, "Issue with statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    printf("1\n");
    sqlite3_bind_int( make_new_contact,   1, max_id() + 1);
    printf("2\n");
    sqlite3_bind_text(make_new_contact,   2, con->con->name       , -1, SQLITE_STATIC);
    printf("3\n");
    sqlite3_bind_text(make_new_contact,   3, con->con->number     , -1, SQLITE_STATIC);
    printf("4\n");
    sqlite3_bind_text(make_new_contact,   4, con->con->email      , -1, SQLITE_STATIC);
    printf("5\n");
    sqlite3_bind_text(make_new_contact,   5, con->con->org        , -1, SQLITE_STATIC);
    printf("6\n");
    sqlite3_bind_text(make_new_contact,   6, con->con->address    , -1, SQLITE_STATIC);
    printf("7\n");
    sqlite3_bind_text(make_new_contact,   7, con->con->photoloc   , -1, SQLITE_STATIC);
    printf("8\n");

    if (sqlite3_step(make_new_contact) != SQLITE_DONE) {
        fprintf(stderr, "Issue with statement: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(make_new_contact);
}

int db_init() {
    sqlite3 *db;
    int rc;
    char *err;

    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        printf("Database couldn't open!\n%s\n", sqlite3_errmsg(db));
        return rc;
    }

    const char *database_schema = "CREATE TABLE IF NOT EXISTS contacts (" \
                                  "ID INT PRIMARY KEY NOT NULL, " \ 
                                  "NAME TEXT NOT NULL, " \
                                  "NUMBER TEXT, " \
                                  "EMAIL TEXT, " \ 
                                  "ORG TEXT, " \
                                  "ADDRESS TEXT, " \
                                  "PHOTOLOC TEXT " \
                                  ");";

    rc = sqlite3_exec(db, database_schema, NULL, 0, &err);
    if (rc) {
        printf("Database couldn't open!\n%s\n", err);
        return rc;
    }

    sqlite3_close(db);

    return 0;
}
