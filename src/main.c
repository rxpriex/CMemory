#include "Memory.h"

#include <stdio.h>

int main(int argc, char **argv) {

    int *i = rxalloc(sizeof(int));
    int *i1 = rxalloc(sizeof(int));
    *i = 5;

    //get_debug_outputs();

    rxfree(i);

    //get_debug_outputs();

    while (1) {
        void *vd = rxalloc(16);
        rxfree(vd);
    }

    return 0;
}