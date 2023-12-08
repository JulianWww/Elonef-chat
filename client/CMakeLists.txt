file(GLOB_RECURSE source src/*)

add_executable(client main.cpp ${source})
target_include_directories(client PUBLIC include/)
target_link_libraries(client elonef-communication CLI11 elonef-chat-share)

set_property(TARGET client PROPERTY CXX_STANDARD 23)

add_compile_definitions(KEY_FILE=".private_key")
add_compile_definitions(EXE_NAME="client")