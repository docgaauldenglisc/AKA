#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#include "gui.h"
#include "database.h"

int g_search_count = 0;
idList g_ids = {.ids = NULL, .id_amount = 0};

void db_save_contact(ContactText *con);
void db_edit_contact(ContactText *con);
idList db_search(char *query);
char *db_get(char *col, int row);
int db_max_id();
int db_init();

static int search_callback(void *data, int argc, char **argv, char **nu) {
    char **temp = (char **)data;
    unsigned long length = strlen(argv[0]);
    *temp = malloc(sizeof(char) * length);

    if (argc > 0 && argv[0] != NULL) {
        snprintf(*temp, length + 1, "%s", argv[0]);
    }
    else {
        *temp = "\0";
    }
    return 0;
}

static int search_set_count(void *data, int argc, char **argv, char **az_col_name) {
    g_ids.id_amount++;

    return 0;
}

static int search_set_ids(void *data, int argc, char **argv, char **az_col_name) {
    g_ids.ids[g_search_count] = atoi(argv[0]);

    g_search_count++;
    return 0;
}

idList db_search(char *query) {
    sqlite3 *db;
    char *err = 0;
    sqlite3_stmt *stmt;

    g_ids.id_amount = 0;
    g_ids.ids = NULL;

    sqlite3_open("Contacts.db", &db);

    const char *search_stmt = "SELECT * FROM Contacts WHERE NAME LIKE ?;";

    char *pattern = sqlite3_mprintf("%s%", query);
    if(sqlite3_prepare_v2(db, search_stmt, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Issue with statement: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_STATIC);
    char *stmt_char = sqlite3_expanded_sql(stmt);

    g_ids.id_amount = 0;
    sqlite3_exec(db, stmt_char, search_set_count, 0, &err);
    g_search_count = 0;
    g_ids.ids = (int *)malloc(g_ids.id_amount* sizeof(int));
    sqlite3_exec(db, stmt_char, search_set_ids, 0, &err);

    return g_ids;
}

char *db_get(char *col, int row) {
    sqlite3 *db;
    int str_size;
    char query[50] = "SELECT %s FROM Contacts WHERE ID = %i";
    char *err = 0;
    char *temp;
    char *row_str;

    row_str = malloc(sizeof(char) * 10);
    snprintf(row_str, 10, "%i", row);

    str_size = strlen(query) + strlen(col) + strlen(row_str);

    sqlite3_open("Contacts.db", &db);

    char *get_stmt = malloc(str_size); 
    snprintf(get_stmt, str_size, query, col, row); 

    sqlite3_exec(db, get_stmt, search_callback, &temp, &err);

    return temp;
}

char *db_get_table_name() {
    sqlite3 *db;
    char *err;
    char *query = "SELECT name FROM sqlite_schema WHERE type = 'table' AND name NOT LIKE 'sqlite_%';";
    char *temp = "";

    sqlite3_open("Contacts.db", &db);

    sqlite3_exec(db, query, search_callback, &temp, &err);

    return temp;
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

void db_edit_contact(ContactText *con) {
    sqlite3 *db;
    sqlite3_stmt *edit_contact;
    int rc;

    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        printf("Database couldn't open!\n%s\n", sqlite3_errmsg(db));
        return;
    }

    char *edit_contact_temp = "UPDATE contacts SET " \
                              "NAME = ?, " \
                              "NUMBER = ?, " \
                              "EMAIL = ?, " \
                              "ORG = ?, " \
                              "ADDRESS = ?, " \
                              "EXTRA = ?, " \
                              "PHOTOLOC = ? " \
                              "WHERE ID = ?;";
    if (sqlite3_prepare_v2(db, edit_contact_temp, -1, &edit_contact, NULL) != SQLITE_OK) {
        printf("Issue with statement: \n%s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_text(edit_contact,   1, con->name      , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact,   2, con->number    , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact,   3, con->email     , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact,   4, con->org       , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact,   5, con->address   , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact,   6, con->extra     , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact,   7, con->photoloc  , -1, SQLITE_STATIC);
    sqlite3_bind_int( edit_contact,   8, atoi(con->id));

    if (sqlite3_step(edit_contact) != SQLITE_DONE) {
        printf("Issue with statement: \n%s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(edit_contact);
}

void db_save_contact(ContactText *con) {
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
                             "EXTRA," \
                             "PHOTOLOC) " \
                             "VALUES(" \
                             "?," \
                             "?," \
                             "?," \
                             "?," \
                             "?," \
                             "?," \
                             "?," \
                             "?);";
    if (sqlite3_prepare_v2(db, new_contact_temp, -1, &make_new_contact, NULL) != SQLITE_OK) {
        printf("Issue with statement: \n%s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int( make_new_contact,   1, db_max_id() + 1);
    sqlite3_bind_text(make_new_contact,   2, con->name       , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   3, con->number     , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   4, con->email      , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   5, con->org        , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   6, con->address    , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   7, con->extra      , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   8, con->photoloc   , -1, SQLITE_STATIC);

    if (sqlite3_step(make_new_contact) != SQLITE_DONE) {
        printf("Issue with statement: \n%s\n", sqlite3_errmsg(db));
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
                                  "EXTRA TEXT, " \
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
