add_library(shared STATIC
        src/shared_global.cpp
        src/shared_utils.cpp
)

target_include_directories(shared PUBLIC include)

