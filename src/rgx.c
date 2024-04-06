#include <stdio.h>
#include <string.h>
#include <pcre.h>

#include "rgx.h"

int rgx_check_phone(char *phone);
int rgx_check_email(char *email);
int rgx_check_address(char *address);

/*
 * Doesn't even use regex, just returns 1 if the name string is empty.
 */
int rgx_check_name(char *name) {
    if (name[0] == '\0') {
        return 1;
    }
    return 0;
}

int rgx_check_phone(char *phone) {
    if (phone[0] == '\0') {
        return 0;
    }
    const char *pattern = "^(\\+[0-9]+|\\+[0-9]+-[0-9]+|)( |)((\\(|)\\d{3}(\\)|))(-| )(.{3})(-| )(.{4})$";
    const char *error;
    int rc;
    pcre *re;
    int error_offset;
    
    re = pcre_compile(pattern, 0, &error, &error_offset, NULL);

    rc = pcre_exec(re, NULL, phone, strlen(phone), 0, 0, NULL, 0);
    if (rc >= 0) {
        return 0;
    }
    else {
        return 1;
    }

    pcre_free(re);
}

int rgx_check_email(char *email) {
    if (email[0] == '\0') {
        return 0;
    }
    const char *pattern = "((^(\\w)+)(@(\\w)+)(\\.[a-zA-Z]+$|\\.[a-zA-Z]+\\.\\w{2}$))";
    const char *error;
    int rc;
    pcre *re;
    int error_offset;
    
    re = pcre_compile(pattern, 0, &error, &error_offset, NULL);

    rc = pcre_exec(re, NULL, email, strlen(email), 0, 0, NULL, 0);
    if (rc >= 0) {
        return 0;
    }
    else {
        return 1;
    }

    pcre_free(re);
}

int rgx_check_address(char *address) {
    if (address[0] == '\0') {
        return 0;
    }
    const char *pattern = "\\d+ ([\\w| ]+|[\\w| ]+,[\\w| ]+), [a-zA-Z]{2,}, [0-9]{5,}";
    const char *error;
    int rc;
    pcre *re;
    int error_offset;
    
    re = pcre_compile(pattern, 0, &error, &error_offset, NULL);

    rc = pcre_exec(re, NULL, address, strlen(address), 0, 0, NULL, 0);
    if (rc >= 0) {
        return 0;
    }
    else {
        return 1;
    }

    pcre_free(re);
}
