#include "event.h"

#include <std/containers/darray.h>

typedef struct Subscriber {
    EventType type;
    event_callback callback;
} Subscriber;

typedef struct EventContext {
    Subscriber *subscriptions;
} EventContext;

static EventContext event_context = {0};

bool event_init() {
    event_context.subscriptions = darray_create(Subscriber);
    return true;
}

void event_shutdown() {
    darray_destroy(event_context.subscriptions);
    event_context.subscriptions = NULL;
}

bool event_subscribe(EventType type, event_callback callback) {
    for (int i = 0; i < darray_length(event_context.subscriptions); ++i) {
        Subscriber *current = &event_context.subscriptions[i];
        if (current->type == type && current->callback == callback) {
            LOG_WARN("Subscriber already exists for event type: %s", type);
            return false;
        }
    }

    Subscriber subscriber = {
            .type = type,
            .callback = callback
    };

    darray_push(event_context.subscriptions, subscriber);

    return true;
}

void event_dispatch(EventType type, EventData data) {
    for (int i = 0; i < darray_length(event_context.subscriptions); ++i) {
        Subscriber *current = &event_context.subscriptions[i];
        if (current->type == type) {
            current->callback(type, data);
        }
    }
}