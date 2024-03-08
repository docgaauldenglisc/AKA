#ifndef DATABASE_H
#define DATABASE_H

// ---- CALLBACK FUNCTIONS ----- //
static int id_search(void *data, int argc, char **argv, char **az_col_name);
static int callback(void *data, int argc, char **argv, char **az_col_name);
static int get_text_from_col_callback(void *data, int argc, char **argv, char **az_col_name);
// ----- REGULAR FUNCTIONS ----- //
idList get_from_search(char *query);
void verify_db();
void fix_schema();
void write_to_file(contact_chars chars);
int get_max_id();
void get_from_col_and_row(char col[], int row, char *found_text);

#endif
