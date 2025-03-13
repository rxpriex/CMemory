#include "Memory.h"

#include <stdio.h>

int main(int argc, char **argv) {

    int* i = rxalloc(sizeof(int));
    *i = 5;

    printf("%i",*i);

    return 0;
}