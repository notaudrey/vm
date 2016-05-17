// TODO: DOES THIS EVEN WORK LOL
#include <stdlib.h>
#include <string.h>

#include "list.h"

struct list_s *list_new(int size) {
    struct list_s *list = malloc(sizeof(struct list_s));
    list->data = malloc(sizeof(void *) * size);
    list->size = 0;
    list->max_size = size;
    return list;
}

/// Note: No bounds checking is done 'cuz we can't return something meaningful
/// since we're returning a void pointer, and we don't have exceptions since
/// this isn't C++. Bounds-checking is left as an exercise for the user.
void *list_get(struct list_s *list, int index) {
    return list->data[index];
}

void list_add(struct list_s *list, void *data) {
    if(list->size == list->max_size - 1) {
        // Reallocate so that we can store more
        list->max_size *= 2;
        list->data = realloc(list->data, list->max_size * sizeof(void *));
    }

    list->data[list->size++] = data;
}

void *list_remove(struct list_s *list, int index) {
    void *remove = list->data[index];
    list->data[index] = NULL;
    memmove(list->data + index, list->data + index + 1, (list->size - index - 1) * sizeof(void *));
    return remove;
}

int list_size(struct list_s *list) {
    return list->size;
}

int list_max_size(struct list_s *list) {
    return list->max_size;
}

