#include <stdio.h>
#include <string.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "conoptions.h"
#include "database.h"

int max_id_val = 0;

static int callback(void *data, int argc, char **argv, char **az_col_name) {
    for (int i = 0; i < argc; ++i) {
        printf("%s = %s\n", az_col_name[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

static int id_search(void *data, int argc, char **argv, char **az_col_name) {
    if (argc > 0 && argv[0]) {
        max_id_val = atoi(argv[0]);
        printf("%i\n", max_id_val);
    }
    else {
        printf("Failed to get id\n");
    }
    return 0;
    //TODO understand this
}

static int get_text_from_col_callback(void *data, int argc, char **argv, char **az_col_name) { 

    if (argc > 0 && argv[0] != NULL) {
        strncpy((char *)data, argv[0], 49);
        ((char *)data)[49] = '\0';
    }
    else {
        *(char *)data = '\0';
        fprintf(stderr, "Nothing copied!");
    }
    return 0;

}

int verify_db() {

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
                                        "ADDRESS " "TEXT " "\n" 
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

void fix_schema() {
    
    

}

void write_to_file(contact_chars chars) {
    
    sqlite3 *db;
    int rc;
    int maxid;

    rc = sqlite3_open("Contacts.db", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: &s\n", sqlite3_errmsg(db));
    }

    maxid = get_max_id(db);

    sqlite3_stmt *insert;
    const char *make_new_contact =  "INSERT INTO Contacts(" \
                                     "ID," \
                                     "NAME," \
                                     "NUMBER," \
                                     "EMAIL," \
                                     "ORG," \
                                     "ADDRESS)" \
                                     "VALUES(" \
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
    sqlite3_bind_int(insert,    1, ++maxid);
    sqlite3_bind_text(insert,   2, chars.name   , -1, SQLITE_STATIC);
    sqlite3_bind_text(insert,   3, chars.number , -1, SQLITE_STATIC);
    sqlite3_bind_text(insert,   4, chars.email  , -1, SQLITE_STATIC);
    sqlite3_bind_text(insert,   5, chars.org    , -1, SQLITE_STATIC);
    sqlite3_bind_text(insert,   6, chars.address, -1, SQLITE_STATIC);

    if (sqlite3_step(insert) != SQLITE_DONE) {
        fprintf(stderr, "Issue with statement: %s\n", sqlite3_errmsg(db));
    }
    else {
        fprintf(stdout, "Contact Added\n");
    }

    sqlite3_finalize(insert);

}

int get_max_id() {
    
    sqlite3 *db;
    int rc;

    rc = sqlite3_open("Contacts.db", &db);

    char *zErrMsg = 0;
    const char *get_most_recent_id = "SELECT MAX(ID) FROM Contacts;";

    rc = sqlite3_exec(db, get_most_recent_id, id_search, 0, &zErrMsg);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error running command: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return -1;
    }
    else {
        printf("Max ID val: %d\n", max_id_val);
        return max_id_val;
    }

    sqlite3_close(db);

}

void get_text_from_col(char col[], int id, char *text_return) {
    
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
    
    snprintf(istr, sizeof(istr), "%i", id);

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
        fprintf(stdout, "%s\n", text);
        strncpy(text_return, text, sizeof(text_return)); 
    }

    sqlite3_close(db);

} 

