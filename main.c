#include <stdio.h>
#include "stack.h"

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("Not enough arguments");
        return 9;
    }

    int res;

    for(int i = 1; i < argc; i++) {
        struct stack_t *stack = NULL;

        res = stack_load(&stack, *(argv + i));
        if(res == 2) {
            printf("Couldn't open file %s\n", *(argv + i));
            continue;
        }
        else if(res == 3) {
            printf("Failed to allocate memory");
            return 8;
        }

        res = stack_save(stack, *(argv + i));
        if(res == 2) {
            printf("Couldn't create file\n");
            stack_destroy(&stack);
            return 5;
        }

        stack_destroy(&stack);
        printf("File %s saved\n", *(argv + i));
    }

    return 0;
}
