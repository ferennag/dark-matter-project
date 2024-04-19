#pragma once

#include <std/std.h>

typedef enum EventType {
    EVENT_KEYDOWN,
    EVENT_KEYUP,
    EVENT_QUIT
} EventType;

/**
 * Event data can store 64 bytes of data.
 */
typedef union EventData {
    u32 data_u32[2];
    u16 data_u16[4];
    u8 data_u8[8];
} EventData;

typedef void (*event_callback)(EventType type, EventData data);

bool event_init();

void event_shutdown();

bool event_subscribe(EventType type, event_callback callback);

void event_dispatch(EventType type, EventData data);
