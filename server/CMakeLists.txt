add_executable(server
    src/server_main.cpp
)

target_include_directories(server PRIVATE include)
target_link_libraries(server PRIVATE shared)
