add_executable(dark_matter_testbed
        src/main.c
)
target_compile_options(dark_matter_testbed PRIVATE -Wall -g -DAPI_IMPORT)
target_link_libraries(dark_matter_testbed std dark_matter_engine)
