#include <gtk/gtk.h>

#include "gui.h"
#include "database.h"

int main(int argc, char **argv) {
    //Make sure the database has been created as a file
    //and can be opened when the program is.
    db_init();

    gui_init(argc, argv);
}
