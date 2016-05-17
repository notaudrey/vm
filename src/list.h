#pragma once

struct list_s {
    void **data;
    int size;
    int max_size;
};

struct list_s *list_new(int size);
void *list_get(struct list_s *, int);
void list_add(struct list_s *, void *);
void *list_remove(struct list_s *, int);
int list_size(struct list_s *);
int list_max_size(struct list_s *);

