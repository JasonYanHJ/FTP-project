cmake_minimum_required(VERSION 3.23)
project(project C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(server
        server/server.c
        common/try_catch.c
        common/try_catch.h
        common/exitable_functions.h
        common/exitable_functions.c)

add_executable(client
        client/client.c
        common/try_catch.c
        common/try_catch.h
        common/exitable_functions.h
        common/exitable_functions.c)
