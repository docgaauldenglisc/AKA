//Standards
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

//Libraries
#include <pcre.h>

//Local Files
#include "rgx.h"

bool string_is_empty(char *str);
int rgx_check_website(char *website);
int rgx_check_filename(char *filename);
int rgc_check_name(char *name);
int rgx_check_phone(char *phone);
int rgx_check_email(char *email);

bool string_is_empty(char *str) {
    if (str[0] == '\0') {
        return true;
    }
    return false; 
}

bool match_str(const char *str, const char *pattern) {
    const char *error;
    int rc;
    pcre *re;
    int error_offset;
    
    re = pcre_compile(pattern, 0, &error, &error_offset, NULL);
    rc = pcre_exec(re, NULL, str, strlen(str), 0, 0, NULL, 0);
    if (rc >= 0) {
        pcre_free(re);
        return true;
    }
    else {
        pcre_free(re);
        return false;
    }
}

int rgx_check_website(char *website) {
    if (string_is_empty(website)) {
        return GOOD;
    }
    const char *pattern = "^(http(s|):\\/\\/|)[\\w]+\\.[\\w]{2,}(|\\.[\\w]{2,})(\\/|)(\\S+|)$";
    if (match_str(website, pattern)) {
        return GOOD;
    }
    else {
        return BAD;
    }
}

int rgx_check_filename(char *filename) {
    //When a file needs to be checked, there can't just not be a file
    //So if the file name is empty, it returns bad.
    if (string_is_empty(filename)) {
        return BAD;
    }
    const char *pattern = "^.+\\.db$";
    if (match_str(filename, pattern)) {
        return GOOD;
    }
    else {
        return BAD;
    }
}

int rgx_check_name(char *name) {
    return (string_is_empty(name));
}

int rgx_check_phone(char *phone) {
    if (string_is_empty(phone)) {
        return GOOD;
    }
    const char *pattern = "^(\\+\\d+|\\+\\d+-\\d+|)( |)((\\(|)\\d{3}(\\)|))(-| )(.{3})(-| )(.{4})$";
    if (match_str(phone, pattern)) {
        return GOOD;
    }
    else {
        return BAD;
    }
}

int rgx_check_email(char *email) {
    if (string_is_empty(email)) {
        return GOOD;
    }
    const char *pattern = "^\\w+@\\w+(\\.[a-zA-Z]+|\\.[a-zA-Z]+\\.\\w{2})$";
    if (match_str(email, pattern)) {
        return GOOD;
    }
    else {
        return BAD;
    }
}
