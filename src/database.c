//Standards
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Libraries
#include <sqlite3.h>
#include <regex.h>

//Local Files
#include "database.h"
#include "rgx.h"

int g_search_count;
idList g_ids;
char *g_file_that_is_open;

idList db_search(char *query);
int db_set_open_file(char *filename);
int db_backup_at(char *filename);
void db_delete_contact(int id);
int db_save_contact(ContactText *con);
int db_edit_contact(ContactText *con);
char *db_get(char *col, int row);
int db_max_id();
int db_init();

static int search_set_count(void *data, int argc, char **argv, char **az_col_name) {
    //This will loop for each result
    g_ids.id_amount++;

    return 0;
}

static int search_set_ids(void *data, int argc, char **argv, char **az_col_name) {
    //This will loop for each result
    g_ids.ids[g_search_count] = atoi(argv[0]);

    g_search_count++;
    return 0;
}

idList db_search(char *query) {
    sqlite3 *db;
    sqlite3_open(g_file_that_is_open, &db);

    //Making the pattern "query%" allows for fuzzy-ish searching rather than whole
    const char *pattern = sqlite3_mprintf("%s%", query); 
    const char *search_query = "SELECT * FROM Contacts WHERE NAME LIKE ?;";
    sqlite3_stmt *search_stmt;
    sqlite3_prepare_v2(db, search_query, -1, &search_stmt, NULL);
    sqlite3_bind_text(search_stmt, 1, pattern, -1, SQLITE_STATIC);

    g_search_count = 0;
    g_ids.id_amount = 0;
    g_ids.ids = NULL;
    char *err = 0;
    char *stmt_char = sqlite3_expanded_sql(search_stmt);
    sqlite3_exec(db, stmt_char, search_set_count, 0, &err);

    g_ids.ids = (int *)malloc(g_ids.id_amount * sizeof(int));
    sqlite3_exec(db, stmt_char, search_set_ids, 0, &err);

    sqlite3_finalize(search_stmt);
    sqlite3_close(db);

    return g_ids;
}

int db_set_open_file(char *filename) {
    if (rgx_check_filename(filename)) {
        return 1;
    }
    unsigned long length = strlen(filename);
    g_file_that_is_open = malloc(sizeof(char) * length);
    g_file_that_is_open = strndup(filename, length);

    return 0;
}

int db_backup_at(char *filename) {
    //This will return if the filename is empty, 
    //or if it doesn't end in ".db"
    if (rgx_check_filename(filename)) {
        return 1;
    }

    int rc;
    sqlite3 *orig_db;
    rc = sqlite3_open(g_file_that_is_open, &orig_db);
    if (rc) {
        puts("Couldn't open the original database");
        return -1;
    }
    sqlite3 *bak_db;
    rc = sqlite3_open(filename, &bak_db);
    if (rc) {
        puts("Couldn't open the backup database");
        return -1;
    }

    const char *database_schema = "CREATE TABLE IF NOT EXISTS contacts (" \
                                  "ID INT PRIMARY KEY NOT NULL, " \ 
                                  "NAME TEXT NOT NULL, " \
                                  "TITLE TEXT, " \
                                  "PHONE TEXT, " \
                                  "EMAIL TEXT, " \ 
                                  "ORG TEXT, " \
                                  "ADDRESS TEXT, " \
                                  "EXTRA TEXT, " \
                                  "PHOTOLOC TEXT " \
                                  ");";

    char *err = 0;
    sqlite3_exec(bak_db, database_schema, NULL, 0, &err);

    const char *attach_db = "ATTACH DATABASE ? AS 'bak';";
    const char *copy_contacts = "INSERT INTO bak.contacts SELECT * FROM contacts;";
    sqlite3_stmt *bak_stmt;
    sqlite3_prepare_v2(orig_db, attach_db, -1, &bak_stmt, NULL);
    sqlite3_bind_text(bak_stmt, 1, filename, -1, SQLITE_STATIC);

    sqlite3_exec(orig_db, sqlite3_expanded_sql(bak_stmt), NULL, 0, &err);
    sqlite3_exec(orig_db, copy_contacts, NULL, 0, &err);

    sqlite3_finalize(bak_stmt);

    sqlite3_close(orig_db);
    sqlite3_close(bak_db);

    return 0;
}

void db_delete_contact(int id) {
    sqlite3 *db;
    sqlite3_open(g_file_that_is_open, &db);

    const char *delete_temp = "UPDATE contacts SET NAME = 'del' WHERE id = ?;";
    sqlite3_stmt *delete_stmt;
    sqlite3_prepare_v2(db, delete_temp, -1, &delete_stmt, NULL);
    sqlite3_bind_int(delete_stmt, 1, id);

    char *err = 0;
    sqlite3_exec(db, sqlite3_expanded_sql(delete_stmt), NULL, 0, &err);

    sqlite3_close(db);
}

static int get_cols(void *data, int argc, char **argv, char **nu) {
    char **temp = (char **)data;
    unsigned long length = strlen(argv[0]);
    *temp = malloc(sizeof(char) * length + 1);

    //if there are results
    if (argc > 0 && argv[0] != NULL) {
        //add them to the return
        snprintf(*temp, length + 1, "%s", argv[0]);
    }
    else {
        //if not, set them to null
        *temp = "\0";
    }
    return 0;
}

char *db_get(char *col, int row) {
    sqlite3 *db;
    sqlite3_open(g_file_that_is_open, &db);

    char *row_str = malloc(sizeof(char) * 10);
    snprintf(row_str, 10, "%i", row);

    const char query[50] = "SELECT %s FROM Contacts WHERE ID = %i";
    int str_size = strlen(query) + strlen(col) + strlen(row_str);
    char *get_stmt = malloc(str_size); 
    //put the search column and row into the query 
    snprintf(get_stmt, str_size, query, col, row); 
    char *err = 0;
    char *temp;
    sqlite3_exec(db, get_stmt, get_cols, &temp, &err);

    sqlite3_close(db);
    free(get_stmt);
    free(row_str);

    return temp;
}

static int get_max_id(void *data, int argc, char **argv, char **nu) {
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
    sqlite3_open(g_file_that_is_open, &db);

    int max_id;
    char *err;
    char *max_query = "SELECT MAX(ID) FROM Contacts";
    sqlite3_exec(db, max_query, get_max_id, &max_id, &err);

    sqlite3_close(db);

    return max_id;
}

int db_edit_contact(ContactText *con) {
    if (rgx_check_phone(con->phone) == 1) {
        return 1;
    }
    if (rgx_check_email(con->email) == 1) {
        return 2;
    }
    if (rgx_check_address(con->address) == 1) {
        return 3;
    }
    if (rgx_check_name(con->name) == 1) {
        return 4;
    }

    sqlite3 *db;
    sqlite3_open(g_file_that_is_open, &db);

    char *edit_contact_query = "UPDATE contacts SET " \
                               "ID = ?, " \
                               "NAME = ?, " \
                               "TITLE = ?, " \
                               "PHONE = ?, " \
                               "EMAIL = ?, " \
                               "ORG = ?, " \
                               "ADDRESS = ?, " \
                               "EXTRA = ?, " \
                               "PHOTOLOC = ? " \
                               "WHERE ID = ?;";
    sqlite3_stmt *edit_contact_stmt;
    sqlite3_prepare_v2(db, edit_contact_query, -1, &edit_contact_stmt, NULL);

    sqlite3_bind_int( edit_contact_stmt,   1, atoi(con->id));
    sqlite3_bind_text(edit_contact_stmt,   2, con->name      , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact_stmt,   3, con->title     , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact_stmt,   4, con->phone     , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact_stmt,   5, con->email     , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact_stmt,   6, con->org       , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact_stmt,   7, con->address   , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact_stmt,   8, con->extra     , -1, SQLITE_STATIC);
    sqlite3_bind_text(edit_contact_stmt,   9, con->photoloc  , -1, SQLITE_STATIC);
    sqlite3_bind_int( edit_contact_stmt,  10, atoi(con->id));

    sqlite3_step(edit_contact_stmt);

    sqlite3_finalize(edit_contact_stmt);
    sqlite3_close(db);

    return 0;
}

int db_save_contact(ContactText *con) {
    sqlite3 *db;
    sqlite3_stmt *make_new_contact;
    int rc;

    if (rgx_check_phone(con->phone) == 1) {
        return 1;
    }
    if (rgx_check_email(con->email) == 1) {
        return 2;
    }
    if (rgx_check_address(con->address) == 1) {
        return 3;
    }
    if (rgx_check_name(con->name) == 1) {
        return 4;
    }

    rc = sqlite3_open(g_file_that_is_open, &db);
    if (rc) {
        printf("Database couldn't open!\n%s\n", sqlite3_errmsg(db));
        return -1;
    }

    char *new_contact_temp = "INSERT INTO Contacts(" \
                             "ID," \
                             "NAME," \
                             "TITLE," \
                             "PHONE," \
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
                             "?," \
                             "?);";
    sqlite3_prepare_v2(db, new_contact_temp, -1, &make_new_contact, NULL);

    sqlite3_bind_int( make_new_contact,   1, db_max_id() + 1);
    sqlite3_bind_text(make_new_contact,   2, con->name      , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   3, con->title     , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   4, con->phone     , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   5, con->email     , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   6, con->org       , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   7, con->address   , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   8, con->extra     , -1, SQLITE_STATIC);
    sqlite3_bind_text(make_new_contact,   9, con->photoloc  , -1, SQLITE_STATIC);

    sqlite3_step(make_new_contact);

    sqlite3_finalize(make_new_contact);
    sqlite3_close(db);

    return 0;
}

int db_init() {
    sqlite3 *db;
    char *err;

    db_set_open_file("Contacts.db");
    sqlite3_open(g_file_that_is_open, &db);

    const char *database_schema = "CREATE TABLE IF NOT EXISTS contacts (" \
                                  "ID INT PRIMARY KEY NOT NULL, " \ 
                                  "NAME TEXT NOT NULL, " \
                                  "TITLE TEXT, " \
                                  "PHONE TEXT, " \
                                  "EMAIL TEXT, " \ 
                                  "ORG TEXT, " \
                                  "ADDRESS TEXT, " \
                                  "EXTRA TEXT, " \
                                  "PHOTOLOC TEXT " \
                                  ");";

    sqlite3_exec(db, database_schema, NULL, 0, &err);

    sqlite3_close(db);

    return 0;
}
