find_package(Git REQUIRED)

function(getlib _name _uri)
    if (NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/${_name}")
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" clone --recurse-submodules ${_uri}
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}"
        )
    else()
        execute_process(
            COMMAND "${GIT_EXECUTABLE}" pull ${_uri}
            WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${_name}"
        )
    endif()
endfunction()

set(CLI11_BUILD_EXAMPLES TRUE)

getlib(elonef-comunication https://github.com/JulianWww/elonef-comunication)
getlib(CLI11 https://github.com/CLIUtils/CLI11)

add_subdirectory(elonef-comunication/cpp)
add_subdirectory(CLI11)


