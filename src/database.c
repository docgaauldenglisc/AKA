#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdbool.h>

#include "conoptions.h"
#include "database.h"

int id_val = 0;

typedef struct {
    int *ids;
    int count;
} idList;

// ----- CALLBACK FUNCTIONS ----- //
static int search_callback(void *data, int argc, char **argv, char **az_col_name) {
    idList *list = (idList *)data;

    if (argc > 0 && argv[0]) {
        int id = atoi(argv[0]);
        list->ids = realloc(list->ids, (list->count + 1) * sizeof(int));

        list->ids[list->count++] = id;
    }
    return 0;
}

static int callback(void *data, int argc, char **argv, char **az_col_name) {
    for (int i = 0; i < argc; ++i) {
        printf("%s = %s\n", az_col_name[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
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
        strncpy((char *)data, argv[0], 49);
        ((char *)data)[49] = '\0';
    }
    else {
        *(char *)data = '\0';
    }
    return 0;
}

// ----- REGULAR FUNCTIONS ----- //
int get_from_search(char *query) {
    sqlite3 *db;
    char *err;
    int rc;
    idList list = {.ids = NULL, .count = 0};

    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        fprintf(stderr, "SQL Error: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_stmt *statement;
    const char *search_database = "SELECT NAME FROM Contacts WHERE NAME LIKE '%?%';";

    if(sqlite3_prepare_v2(db, search_database, -1, &statement, NULL) != SQLITE_OK) {
        fprintf(stderr, "Issue with statement: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_bind_text(statement, 1, query, -1, SQLITE_STATIC); 

    rc = sqlite3_exec(db, search_database, callback, 0, &err);
    return 0;
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
        fprintf(stdout, "%s: %s\n", col, text);
        strncpy(text_return, text, 50); 
    }

    sqlite3_close(db);
} 

