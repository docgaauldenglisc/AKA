#include <stdlib.h>
#include <gtk/gtk.h>

#include "gui.h"
#include "database.h"

int main(int argc, char **argv) {
    db_init();

    gui_init(argc, argv);
}
