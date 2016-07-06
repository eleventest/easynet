
cmake_minimum_required(VERSION 2.8)

set(PROJ_NAME enet)
file(GLOB SRCLIST *.c *.h)

add_library(${PROJ_NAME} ${SRCLIST})

