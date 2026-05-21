add_executable(server

)

target_include_directories(server PRIVATE include)
target_link_libraries(server PRIVATE shared)
