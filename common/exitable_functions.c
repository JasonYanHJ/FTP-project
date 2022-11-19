//
// Created by 严洪钧 on 2022/11/17.
//
#include "exitable_functions.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int e_socket(int domain, int type, int protocol) {
    int ret = socket(domain, type, protocol);
    if (ret < 0)
        error("ERROR opening socket");
    return ret;
}

int e_bind(int socket, const struct sockaddr *address, socklen_t address_len) {
    int ret = bind(socket, address, address_len);
    if (ret < 0)
        error("ERROR on binding");
    return ret;
}

int e_accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len) {
    int ret = accept(socket, address, address_len);
    if (ret < 0)
        error("ERROR on accept");
    return ret;
}

struct hostent *e_gethostbyname(const char *name) {
    struct hostent *ret = gethostbyname(name);
    if (ret == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    return ret;
}

int e_getpeername(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len) {
    int ret = getpeername(socket, address, address_len);
    if (ret < 0)
        error("ERROR get peer name");
    return ret;
}

int e_connect(int socket, const struct sockaddr *address, socklen_t address_len) {
    int ret = connect(socket, address, address_len);
    if (ret < 0)
        error("ERROR connecting");
    return ret;
}

ssize_t e_write(int fd, const void *buf, size_t nbyte) {
    ssize_t ret = write(fd, buf, nbyte);
    if (ret < 0)
        error("ERROR writing to socket");
    return ret;
}

ssize_t e_read(int fd, void *buf, size_t nbyte) {
    ssize_t ret = read(fd, buf, nbyte);
    if (ret < 0)
        error("ERROR reading from socket");
    return ret;
}


pid_t e_fork() {
    pid_t ret = fork();
    if (ret < 0)
        error("ERROR on fork");
    return ret;
}
