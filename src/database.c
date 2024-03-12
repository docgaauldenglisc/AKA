#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdbool.h>

#include "conoptions.h"
#include "database.h"

int search_count = 0;
int id_val = 0;
idList ids = {.ids = NULL, .count = 0};

// ----- CALLBACK FUNCTIONS ----- //
static int callback(void *data, int argc, char **argv, char **az_col_name) {
    return 0;
}

static int id_search(void *data, int argc, char **argv, char **az_col_name) {
    if (argc > 0 && argv[0]) {
        id_val = atoi(argv[0]);
        return 0;
    }
    else {
        fprintf(stderr, "Failed to get number\n");
        return -1;
    }
    return 0;
}

static int get_text_from_col_callback(void *data, int argc, char **argv, char **az_col_name) { 
    if (argc > 0 && argv[0] != NULL) {
        strcpy((char *)data, argv[0]);
        ((char *)data)[4095] = '\0';
    }
    else {
        *(char *)data = '\0';
    }
    return 0;
}

static int search_set_count(void *data, int argc, char **argv, char **az_col_name) {
    ids.count++;

    return 0;
}

static int search_set_ids(void *data, int argc, char **argv, char **az_col_name) {
    ids.ids[search_count] = atoi(argv[0]);

    search_count++;
    return 0;
}

// ----- REGULAR FUNCTIONS ----- //
idList get_from_search(char *query) {
    sqlite3 *db;
    char *err = 0;
    sqlite3_stmt *stmt;

    ids.count = 0;
    ids.ids = NULL;

    sqlite3_open("Contacts.db", &db);

    const char *search_stmt = "SELECT * FROM Contacts WHERE NAME LIKE ?;";

    char *pattern = sqlite3_mprintf("%s%", query);
    if(sqlite3_prepare_v2(db, search_stmt, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Issue with statement: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_STATIC);
    char *stmt_char = sqlite3_expanded_sql(stmt);

    ids.count = 0;
    sqlite3_exec(db, stmt_char, search_set_count, 0, &err);
    search_count = 0;
    ids.ids = (int *)malloc(ids.count * sizeof(int));
    sqlite3_exec(db, stmt_char, search_set_ids, 0, &err);

    return ids;
}

void verify_db() {
    // Make the contact database
    sqlite3 *db;
    char *zErrMsg = 0; // zero terminated string, error message
    int rc; // Return Code
           
    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: &s\n", sqlite3_errmsg(db));
    }

    const char *make_contact_table =    "CREATE TABLE IF NOT EXISTS Contacts (\n"  
//                                      "col"      "type"  "attributes"
                                        "ID "      "INT "  "PRIMARY KEY NOT NULL,\n" 
                                        "NAME "    "TEXT " "NOT NULL,\n" 
                                        "NUMBER "  "TEXT " ",\n" 
                                        "EMAIL "   "TEXT " ",\n" 
                                        "ORG "     "TEXT " ",\n" 
                                        "ADDRESS " "TEXT " ",\n" 
                                        "PHOTOLOC ""TEXT " "\n" 
                                        ");" ;
    
    rc = sqlite3_exec(db, make_contact_table, callback, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Problem with SQL: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else { 
        fprintf(stdout, "Table Created\n");
    }

    sqlite3_close(db);
}

void edit_col_and_row(contact_chars *chars) {
    sqlite3 *db;
    int rc;
    sqlite3_stmt *stmt;

    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        printf("SHIIIIIT!\n");
    }

    const char *statement_string = "UPDATE Contacts" \
                                   "SET NAME = \'?\'," \ 
                                   "NUMBER = \'?\'," \
                                   "EMAIL = \'?\'," \
                                   "ORG = \'?\'," \
                                   "ADDRESS = \'?\'," \
                                   "PHOTOLOC = \'?\'," \
                                   "WHERE ID = \'?\'";
    sqlite3_prepare_v2(db, statement_string, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, chars->name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, chars->number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, chars->email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, chars->org, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, chars->address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, chars->photoloc, -1, SQLITE_STATIC);
}

void write_to_file(contact_chars chars) {
    sqlite3 *db;
    int rc;
    int maxid;

    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: &s\n", sqlite3_errmsg(db));
    }

    maxid = get_max_id();
    printf("Max: %d\n", maxid);

    sqlite3_stmt *insert;
    const char *make_new_contact =  "INSERT INTO Contacts(" \
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
    if(sqlite3_prepare_v2(db, make_new_contact, -1, &insert, NULL) != SQLITE_OK) {
        fprintf(stderr, "Issue with statement: %s\n", sqlite3_errmsg(db));
        return;
    }
    sqlite3_bind_int(insert,    1, get_max_id() + 1);
    sqlite3_bind_text(insert,   2, chars.name   , -1, SQLITE_STATIC);
    sqlite3_bind_text(insert,   3, chars.number , -1, SQLITE_STATIC);
    sqlite3_bind_text(insert,   4, chars.email  , -1, SQLITE_STATIC);
    sqlite3_bind_text(insert,   5, chars.org    , -1, SQLITE_STATIC);
    sqlite3_bind_text(insert,   6, chars.address, -1, SQLITE_STATIC);
    sqlite3_bind_text(insert,   7, chars.photoloc,-1, SQLITE_STATIC);

    if (sqlite3_step(insert) != SQLITE_DONE) {
        fprintf(stderr, "Issue with statement: %s\n", sqlite3_errmsg(db));
    }
    else {
        fprintf(stdout, "Contact Added\n");
    }

    sqlite3_finalize(insert);
}

int get_max_id() {
    char *zErrMsg = 0;
    char query[100];
    int rc;
    sqlite3 *db;

    if (db == NULL) {
        sqlite3 *replacement_db;
        rc = sqlite3_open("Contacts.db", &replacement_db);
    }
    else {
        rc = sqlite3_open("Contacts.db", &db);
    }

    strcpy(query, "SELECT \0");

    strncat(query, "MAX(ID) ", 10);

    strncat(query, "FROM Contacts;\0", 20);

    rc = sqlite3_exec(db, query, id_search, 0, &zErrMsg);

    printf("Max ID val: %d\n", id_val);
    sqlite3_close(db);
    return id_val;
}

void get_from_col_and_row(char *col, int row, char *text_return) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char query[100];
    char text[100];
    char istr[10];

    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        fprintf(stderr, "DATABASE STUCK! : %s\n", sqlite3_errmsg(db));
    }
    
    snprintf(istr, sizeof(istr), "%i", row);

    strcpy(query, "SELECT "); 
    strncat(query, col, 10);
    strncat(query, " FROM Contacts WHERE ID = ", 30);
    strncat(query, istr, 10);
    strncat(query, ";", 2);

    rc = sqlite3_exec(db, query, get_text_from_col_callback, (void *)text, &zErrMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Problem with SQL: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else {
        strcpy(text_return, text); 
    }

    sqlite3_close(db);
} 

