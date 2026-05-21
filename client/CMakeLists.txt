add_executable(client WIN32
        src/client_main.cpp
        include/client_global.h
        src/client_core.cpp
        include/client_core.h
        src/client_physics.cpp
        include/client_physics.h
)

target_include_directories(client PRIVATE include)
target_link_libraries(client PRIVATE shared)
