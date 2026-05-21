add_executable(client WIN32

)

target_include_directories(client PRIVATE include)
target_link_libraries(client PRIVATE shared)
