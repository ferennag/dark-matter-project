include(FetchContent)

FetchContent_Declare(SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.2
)
FetchContent_MakeAvailable(SDL2)

find_package(Vulkan)

add_library(dark_matter_engine SHARED
        src/engine/renderer/renderer.h
        src/engine/renderer/renderer.c
        src/engine/platform/platform.h
        src/engine/platform/platform_sdl.c
        src/engine/application.c
        src/engine/application.h
        src/engine/entry.h
        src/engine/game_types.h
        src/engine/core/event.c
        src/engine/core/event.h
        src/engine/core/input.h
        src/engine/renderer/vulkan/vulkan_renderer.c
        src/engine/renderer/vulkan/vulkan_renderer.h
        src/engine/renderer/vulkan/vulkan_types.h
)
target_compile_options(dark_matter_engine PRIVATE -Wall -g)
target_include_directories(dark_matter_engine PUBLIC src)
target_link_libraries(dark_matter_engine PUBLIC std SDL2::SDL2 Vulkan::Vulkan)