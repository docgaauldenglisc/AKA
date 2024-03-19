#include <stdio.h>
#include <sqlite3.h>

#include "gui.h"
#include "database.h"

static int search_callback(void *data, int argc, char **argv, char **nu) {
    char *ret = (char *)data;

    ret = argv[0];
    printf("%s\n", ret);

    return 0;
}

static int iterator_callback(void *data, int argc, char **argv, char **nu) {
    int *max_id = (int *)data;

    if (argv[0] == NULL) {
        *max_id = 0;
    }
    else {
        *max_id = atoi(argv[0]);
    }

    return 0;
}

char *db_get(char *col, int row) {
    sqlite3 *db;
    sqlite3_stmt *get_stmt;
    char *err = 0;
    char *ret;

    sqlite3_open("Contacts.db", &db);

    char *get_temp = "SELECT ? FROM Contacts WHERE ID = ?;";
    sqlite3_prepare_v2(db, get_temp, -1, &get_stmt, NULL);
    sqlite3_bind_text(get_stmt, 1, col, -1, SQLITE_STATIC);
    sqlite3_bind_int(get_stmt, 2, row);

    sqlite3_exec(db, (char *)get_stmt, search_callback, &ret, &err);

    sqlite3_finalize(get_stmt);

    return ret;
}

int db_max_id() {
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
    sqlite3_bind_int( make_new_contact,   1, db_max_id() + 1);
    sqlite3_bind_text(make_new_contact,   2, con->con->name       , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   3, con->con->number     , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   4, con->con->email      , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   5, con->con->org        , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   6, con->con->address    , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   7, con->con->photoloc   , -1, SQLITE_STATIC);

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
