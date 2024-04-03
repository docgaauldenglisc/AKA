#include <stdio.h>
#include <string.h>
#include <pcre.h>

#include "rgx.h"

int rgx_test();
int rgx_check_phone(char *phone);
int rgx_check_email(char *email);
int rgx_check_address(char *address);

int rgx_check_phone(char *phone) {
    if (phone[0] == '\0') {
        return 0;
    }
    const char *pattern = "((\\(|)\\d{3}(\\)|))(-| )(.{3})(-| )(.{4})";
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
