#include "list.h"

#include "std/core/memory.h"

Node *node_init(void *data, u64 size) {
    Node *node = memory_alloc(sizeof(Node));
    node->data = data;
    node->size = size;
    return node;
}

void node_destroy(Node *node) {
    memory_free(node->data);
    node->data = NULL;

    memory_free(node);
}

void insert_node_between(Node *prev, Node *next, Node *node) {
    prev->next = node;
    next->prev = node;

    node->prev = prev;
    node->next = next;
}

void remove_node(Node *node) {
    Node *prev = node->prev;
    Node *next = node->next;

    prev->next = next;
    next->prev = prev;
}

List *list_init() {
    List *list = memory_alloc(sizeof(List));

    list->head = node_init(NULL, 0);
    list->tail = node_init(NULL, 0);

    list->head->next = list->tail;
    list->tail->prev = list->head;

    return list;
}

void list_destroy(List *list) {
    Node *current = list->head->next;

    while (current != list->tail) {
        Node *next = current->next;
        node_destroy(current);
        current = next;
    }

    node_destroy(list->head);
    node_destroy(list->tail);
    memory_free(list);
}

void list_push(List *list, void *data, u64 size) {
    void *copy = memory_alloc(size);
    memory_copy(copy, data, size);
    Node *node = node_init(copy, size);
    insert_node_between(list->tail->prev, list->tail, node);
}

void list_pop(List *list, void *out_data) {
    Node *last = list->tail->prev;

    if (last == list->head) {
        return;
    }

    remove_node(last);
    memory_copy(out_data, last->data, last->size);
    node_destroy(last);
}

u64 list_length(List *list) {
    Node *current = list->head->next;

    u64 length = 0;
    while (current != list->tail) {
        ++length;
        current = current->next;
    }

    return length;
}

Node *list_it_first(List *list) {
    if (list->head->next == list->tail) {
        return NULL;
    } else {
        return list->head->next;
    }
}

Node *list_it_next(Node *node) {
    if (node == NULL) {
        return NULL;
    }

    return node->next;
}

bool list_it_is_last(List *list, Node *node) {
    return node == NULL || list->tail == node;
}

void list_foreach(List *list, void (*func)(Node *node, u64 index)) {
    u32 i = 0;
    for (Node *it = list_it_first(list); !list_it_is_last(list, it); it = list_it_next(it)) {
        func(it, i++);
    }
}
