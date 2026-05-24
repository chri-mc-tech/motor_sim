add_executable(client
        src/client_main.cpp
        include/client_global.h
        src/client_core.cpp
        include/client_core.h
        src/client_physics.cpp
        include/client_physics.h
        src/client_vehicles.cpp
        include/client_vehicles.h
        src/client_logger.cpp
        include/client_logger.h
        src/client_ffb.cpp
        include/client_ffb.h
        src/client_network.cpp
        include/client_network.h

)

target_include_directories(client PRIVATE include)
target_link_libraries(client PRIVATE shared)
if(WIN32)
    target_link_libraries(client PRIVATE dinput8 dxguid)
endif()
