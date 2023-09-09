#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int stack_push(struct stack_t **stack, char *value) {
    if(stack == NULL || value == NULL) {
        return 1;
    }

    struct stack_t *new_stack = calloc(1, sizeof(struct stack_t));
    if(new_stack == NULL) {
        return 2;
    }

    new_stack->sentence = value;

    if(*stack == NULL) {
        *stack = new_stack;
    }
    else {
        new_stack->prev = *stack;
        *stack = new_stack;
    }

    return 0;
}

char *stack_pop(struct stack_t **stack, int *err_code) {
    if(stack == NULL || *stack == NULL) {
        if(err_code != NULL) *err_code = 1;
        return NULL;
    }

    struct stack_t *stack_cp = *stack;
    *stack = (*stack)->prev;
    char *cp = stack_cp->sentence;
    free(stack_cp);
    stack_cp = NULL;

    if(err_code != NULL) *err_code = 0;
    return cp;
}

void stack_destroy(struct stack_t** stack) {
    if (stack == NULL || *stack == NULL) {
        return;
    }

    while (*stack != NULL) {
        struct stack_t *stack_cp = *stack;
        *stack = (*stack)->prev;
        free(stack_cp->sentence);
        free(stack_cp);
    }
}

int stack_load(struct stack_t **stack, const char *filename) {
    if(!stack || !filename) return 1;

    if(*stack != NULL) return 1;

    FILE *file = fopen(filename, "rt");
    if(file == NULL) {
        return 2;
    }

    char c;
    int start = -1, end = -1, word_size;

    while(!feof(file)) {
        c = (char)fgetc(file);
        if(c != EOF && c != '.' && start == -1) {
            start = ftell(file);
            start--;
            end = start;
        }
        else if(c != EOF && c != '.' && start != -1) {
            end++;
        }
        else if(c == '.') {
            fseek(file, start, SEEK_SET);

            if(start == -1) {
                fseek(file, end, SEEK_SET);
                word_size = 2;

                struct stack_t *new_stack = malloc(sizeof(struct stack_t));
                if(new_stack == NULL) {
                    return 3;
                }

                new_stack->sentence = calloc(word_size, sizeof(char));
                if(new_stack->sentence == NULL) {
                    free(new_stack);
                    new_stack = NULL;
                    return 3;
                }

                strcpy(new_stack->sentence, ".");
                new_stack->prev = *stack;
                *stack = new_stack;

                start = -1;
                end += 2;
                fseek(file, end, SEEK_SET);
                continue;
            }
            else {
                end++;
                word_size = (end - start + 2);
            }

            struct stack_t *new_stack = malloc(sizeof(struct stack_t));
            if(new_stack == NULL) {
                fclose(file);
                stack_destroy(stack);
                return 3;
            }

            new_stack->sentence = calloc(word_size, sizeof(char));
            if(new_stack->sentence == NULL) {
                free(new_stack);
                new_stack = NULL;
                stack_destroy(stack);
                fclose(file);
                return 3;
            }

            for(int i = 0; i < word_size - 1; i++) {
                *(new_stack->sentence + i) = fgetc(file);
            }

            new_stack->prev = *stack;
            *stack = new_stack;

            fseek(file, end + 1, SEEK_SET);

            start = -1;
        }
    }

    fclose(file);

    return 0;
}

int stack_save(const struct stack_t *stack, const char *filename) {
    if(stack == NULL || filename == NULL) return 1;

    FILE *file = fopen(filename, "w");
    if(file == NULL) {
        return 2;
    }

    const struct stack_t *new_stack = stack;

    do {
        fprintf(file, "%s", new_stack->sentence);
        new_stack = new_stack->prev;
    } while(new_stack->prev != NULL);

    fprintf(file, "%s", new_stack->sentence);

    fclose(file);
    return 0;
}
