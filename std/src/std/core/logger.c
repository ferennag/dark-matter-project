#include "logger.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "logger.h"

bool init_logging() {
    // TODO create a log file
    return true;
}

void shutdown_logging() {
    // TODO cleanup/write queued entries
}

void log_output(LogLevel level, const char *message, ...) {
    const char *level_strings[6] = {"[FATAL]: ", "[ERROR]: ", "[WARN]: ", "[INFO]: ", "[DEBUG]: ", "[TRACE]: " };
//    b8 is_error = level < 2;

    char out_message[32000];
    memset(out_message, 0, sizeof(out_message));

    va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    // TODO platform specific output
    printf("%s%s\n", level_strings[level], out_message);
}