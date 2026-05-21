cmake_minimum_required(VERSION 3.30)
project(game_v4)

set(CMAKE_CXX_STANDARD 20)

find_package(unofficial-enet CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_subdirectory(shared)
add_subdirectory(client)
add_subdirectory(server)

target_link_libraries(server PRIVATE
        unofficial::enet::enet
        yaml-cpp::yaml-cpp
)

target_link_libraries(client PRIVATE
        unofficial::enet::enet
        yaml-cpp::yaml-cpp
        raylib
        glfw
)

target_link_libraries(shared PRIVATE
        unofficial::enet::enet
        yaml-cpp::yaml-cpp
)