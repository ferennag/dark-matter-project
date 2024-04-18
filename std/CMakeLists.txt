add_library(std STATIC src/std/std.c
        src/std/defines.h
        src/std/containers/darray.h
        src/std/containers/darray.c
        src/std/core/memory.h
        src/std/core/logger.h
        src/std/core/memory.c
        src/std/core/logger.c
        src/std/containers/list.h
        src/std/containers/list.c
        src/std/core/file.c
        src/std/core/file.h)
target_include_directories(std PUBLIC src)