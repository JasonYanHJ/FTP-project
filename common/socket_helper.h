//
// Created by 严洪钧 on 2022/11/19.
//

#ifndef PROJECT_SOCKET_HELPER_H
#define PROJECT_SOCKET_HELPER_H
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "exitable_functions.h"
#include "defines.h"
#include "try_catch.h"

void e_start_listen(int *socket, int port_no);
void e_socket_connect(int *socket, struct sockaddr_in dest_addr);
void e_socket_accept(int *socket, int socket_listen);

struct sockaddr_in init_socket_addr_in(int port_no, in_addr_t s_addr);
struct sockaddr_in e_get_peer_addr(int socket);
struct sockaddr_in e_get_server_addr(const char *hostname, int port_no);

void e_write_code(int socket, int code);
int e_read_code(int socket);
#define t_expect_read_code(socket, code) ({ int _code = e_read_code(socket); ASSERT(_code == code, _code); })

#endif //PROJECT_SOCKET_HELPER_H
