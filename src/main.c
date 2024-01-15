#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sqlite3.h>

#include "conoptions.h"
#include "database.h"
#include "config.h"
#include "gui.h"


int main (int argc, char *argv[]) {
    
    if (check_for_config() == 1) {
        make_config();
    }

    verify_db();

    start_gui(argc, argv);
}
