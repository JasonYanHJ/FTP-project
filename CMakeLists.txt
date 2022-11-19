cmake_minimum_required(VERSION 3.23)
project(project C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(server
        server/server.c
        common/try_catch.c
        common/try_catch.h
        common/exitable_functions.h
        common/exitable_functions.c
        common/request.c
        common/request.h
        common/defines.h
        common/file_io_helper.c
        common/file_io_helper.h
        common/defines.h
        common/transfer_packet.h
        common/path_helper.c
        common/path_helper.h
        common/socket_helper.c
        common/socket_helper.h)

add_executable(client
        client/client.c
        common/try_catch.c
        common/try_catch.h
        common/exitable_functions.h
        common/exitable_functions.c
        common/request.c
        common/request.h
        common/defines.h
        common/file_io_helper.c
        common/file_io_helper.h
        common/defines.h
        common/transfer_packet.h
        common/path_helper.c
        common/path_helper.h
        common/socket_helper.c
        common/socket_helper.h)
