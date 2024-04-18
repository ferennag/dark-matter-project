#pragma once

#include "std/defines.h"

typedef struct Node {
    struct Node *prev;
    struct Node *next;

    /// User provided data
    void *data;

    /// The size of the data field
    u64 size;
} Node;

typedef struct List {
    Node *head;
    Node *tail;
} List;

List *list_init();

void list_destroy(List *list);

void list_push(List *list, void *data, u64 size);

void list_pop(List *list, void *out_data);

u64 list_length(List *list);

Node *list_it_first(List *list);

Node *list_it_next(Node *node);

bool list_it_is_last(List *list, Node *node);

void list_foreach(List *list, void (*f)(Node *node, u64 index));
