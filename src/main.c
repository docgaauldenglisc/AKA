#include "gui.h"
#include "database.h"

int main(int argc, char **argv) {
    db_init();

    gui_init(argc, argv);
}
