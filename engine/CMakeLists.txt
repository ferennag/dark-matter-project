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
        src/engine/renderer/vulkan/vulkan_instance.c
        src/engine/renderer/vulkan/vulkan_instance.h
        src/engine/renderer/vulkan/debug_messenger.h
        src/engine/renderer/vulkan/debug_messenger.c
        src/engine/renderer/vulkan/physical_device.c
        src/engine/renderer/vulkan/physical_device.h
        src/engine/renderer/vulkan/device.c
        src/engine/renderer/vulkan/device.h
        src/engine/renderer/vulkan/device.h
        src/engine/renderer/vulkan/device.h
        src/engine/renderer/vulkan/swapchain.c
        src/engine/renderer/vulkan/swapchain.h
        src/engine/renderer/vulkan/frame_buffer.c
        src/engine/renderer/vulkan/frame_buffer.h
        src/engine/renderer/vulkan/graphics_pipeline.c
        src/engine/renderer/vulkan/graphics_pipeline.h
)
target_compile_options(dark_matter_engine PRIVATE -Wall -g -DAPI_EXPORT)
target_include_directories(dark_matter_engine PUBLIC src)
target_link_libraries(dark_matter_engine PUBLIC std SDL2::SDL2 Vulkan::Vulkan)
add_compile_definitions(RENDERER=VULKAN)

IF (WIN32)
add_custom_command(TARGET dark_matter_engine
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/dark_matter_engine.dll $<TARGET_FILE_DIR:dark_matter_testbed>
)
ENDIF ()