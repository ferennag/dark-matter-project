#pragma once

#include "std/defines.h"

#define LOG_WARN_ENABLED TRUE
#define LOG_INFO_ENABLED TRUE


#if DEBUG == TRUE
#define LOG_DEBUG_ENABLED TRUE
#define LOG_TRACE_ENABLED TRUE
#else
#define LOG_DEBUG_ENABLED FALSE
#define LOG_TRACE_ENABLED FALSE
#endif


typedef enum log_level {
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE,
} LogLevel;

bool init_logging();
void shutdown_logging();
void log_output(LogLevel level, const char *message, ...);

#define LOG_FATAL(message, ...) log_output(LOG_LEVEL_FATAL, message, ##__VA_ARGS__)

#ifndef LOG_ERROR
#define LOG_ERROR(message, ...) log_output(LOG_LEVEL_ERROR, message, ##__VA_ARGS__)
#endif

#if LOG_WARN_ENABLED == TRUE
#define LOG_WARN(message, ...) log_output(LOG_LEVEL_WARN, message, ##__VA_ARGS__)
#else
#define LOG_WARN(message, ...)
#endif

#if LOG_INFO_ENABLED == TRUE
#define LOG_INFO(message, ...) log_output(LOG_LEVEL_INFO, message, ##__VA_ARGS__)
#else
#define LOG_INFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == TRUE
#define LOG_DEBUG(message, ...) log_output(LOG_LEVEL_DEBUG, message, ##__VA_ARGS__)
#else
#define LOG_DEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == TRUE
#define LOG_TRACE(message, ...) log_output(LOG_LEVEL_TRACE, message, ##__VA_ARGS__)
#else
#define LOG_TRACE(message, ...)
#endif
