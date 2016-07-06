
cmake_minimum_required(VERSION 2.8)

set(PROJ_NAME easytest)

file(GLOB SRCLIST *.c *.h)
add_executable(${PROJ_NAME} ${SRCLIST})

include_directories ("./lib")

add_subdirectory(lib)

target_link_libraries(${PROJ_NAME} enet)

target_link_libraries(${PROJ_NAME} pthread)
target_link_libraries(${PROJ_NAME} m)
target_link_libraries(${PROJ_NAME} dl)

