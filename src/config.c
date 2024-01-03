#include <stdio.h>
#include <fcntl.h>

#include "config.h"

int add(int a, int b) {
    
    return a + b;

}

int check_for_config() {

    FILE *config;

    config = fopen("hello/example.txt", "r");
    if (config == NULL) {
        fprintf(stderr, "Config couldn't open!\n");
        return 1;
    }

    fprintf(stdout, "Config opened\n");

}

int make_config() {
    // Because this only runs if the config does not exist, it will not lock the file
    FILE *config;
    
    config = fopen("hello/example.txt", "w");
    if (config == NULL) {
        fprintf(stdout, "Config doesn't having a containing folder, creating it...\n");
        mkdir("hello", 00755);
        fprintf(stdout, "Created! Trying again...\n");
    }

    config = fopen("hello/example.txt", "w");
    fclose(config);
    config = fopen("hello/example.txt", "r");
    if (config == NULL) {
        fprintf(stdout, "Couldn't create config!\n");
        return 1;
    }

    fprintf(stdout, "Config created\n");

    fclose(config);

    return 0;

}

void do_config() {
    


}
