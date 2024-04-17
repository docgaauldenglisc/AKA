#ifndef RGX_H
#define RGX_H

enum {
    GOOD = 0,
    BAD,
};

int rgx_check_website(char *website);
int rgx_check_filename(char *name);
int rgx_check_name(char *name);
int rgx_check_phone(char *phone);
int rgx_check_email(char *email);

#endif
