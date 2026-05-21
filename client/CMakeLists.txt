add_executable(client WIN32
    src/client_main.cpp
)

target_include_directories(client PRIVATE include)
target_link_libraries(client PRIVATE shared)
