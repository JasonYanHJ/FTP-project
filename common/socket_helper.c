//
// Created by 严洪钧 on 2022/11/19.
//

#include "socket_helper.h"

void e_start_listen(int *socket, int port_no) {
    *socket = e_socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = init_socket_addr_in(port_no, INADDR_ANY);
    e_bind(*socket, (struct sockaddr *) &addr, sizeof(addr));
    listen(*socket, 5);
}

void e_socket_connect(int *socket, struct sockaddr_in dest_addr) {
    *socket = e_socket(AF_INET, SOCK_STREAM, 0);
    e_connect(*socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
}

void e_socket_accept(int *socket, int socket_listen) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    *socket = e_accept(socket_listen, (struct sockaddr *) &client_addr, &len);
}


struct sockaddr_in init_socket_addr_in(int port_no, in_addr_t s_addr) {
    struct sockaddr_in addr;
    bzero((char *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_no);
    addr.sin_addr.s_addr = s_addr;
    return addr;
}

struct sockaddr_in e_get_peer_addr(int socket) {
    char host[1024];
    struct sockaddr_in client_addr;
    socklen_t len = sizeof client_addr;
    e_getpeername(socket, (struct sockaddr*)&client_addr, &len);
    inet_ntop(AF_INET, &client_addr.sin_addr, host, sizeof(host));

    return init_socket_addr_in(CLIENT_PORT, inet_addr(host));
}

struct sockaddr_in e_get_server_addr(const char *hostname, int port_no) {
    struct hostent *host = e_gethostbyname(hostname);
    struct sockaddr_in host_addr;
    bzero((char *) &host_addr, sizeof(host_addr));
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(port_no);
    bcopy((char *) host->h_addr,
          (char *) &host_addr.sin_addr.s_addr,
          host->h_length);
    return host_addr;
}