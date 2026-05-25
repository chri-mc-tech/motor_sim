add_executable(server
    src/server_main.cpp
        src/server_core.cpp
        include/server_core.h
        src/server_global.cpp
        include/server_global.h
        src/server_logger.cpp
        include/server_logger.h
        src/server_network.cpp
        include/server_network.h
        src/server_config.cpp
        include/server_config.h
        src/server_console.cpp
        include/server_console.h
        src/server_commands.cpp
        include/server_commands.h
        src/server_utils.cpp
        include/server_utils.h
)

target_include_directories(server PRIVATE include)
target_link_libraries(server PRIVATE shared)
