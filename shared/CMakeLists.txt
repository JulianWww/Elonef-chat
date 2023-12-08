file(GLOB_RECURSE source src/*)

add_library(elonef-chat-share ${source})
target_include_directories(elonef-chat-share PUBLIC include/)
target_link_libraries(elonef-chat-share elonef-communication)

set_property(TARGET elonef-chat-share PROPERTY CXX_STANDARD 23)