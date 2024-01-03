#ifndef DATABASE_H
#define DATABASE_H

extern int max_id_val;

static int id_search(void *data, int argc, char **argv, char **az_col_name);
static int callback(void *data, int argc, char **argv, char **az_col_name);
static int get_text_from_col_callback(void *data, int argc, char **argv, char **az_col_name);
int verify_db();
void fix_schema();
void write_to_file(contact_chars chars);
int get_max_id();
void get_text_from_col(char col[], int id, char *found_text);

#endif
