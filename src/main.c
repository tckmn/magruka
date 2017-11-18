#include "magruka.h"

int main(int argc, char **argv) {
    struct magruka *magruka = magruka_init();
    if (!magruka) return 1;
    magruka_main_loop(magruka);
    magruka_destroy(magruka);
}
