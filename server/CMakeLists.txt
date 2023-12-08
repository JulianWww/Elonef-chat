file(GLOB_RECURSE source src/*)

add_executable(server main.cpp ${source})
target_include_directories(server PUBLIC include/)
add_compile_options(-Wall -L/usr/lib)
target_link_libraries( server mysqlcppconn elonef-communication elonef-chat-share)
target_include_directories(server PUBLIC /usr/include/cppconn/)

set_property(TARGET server PROPERTY CXX_STANDARD 23)