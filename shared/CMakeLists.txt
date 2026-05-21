add_library(shared STATIC
    src/shared_global.cpp
)

target_include_directories(shared PUBLIC include)

