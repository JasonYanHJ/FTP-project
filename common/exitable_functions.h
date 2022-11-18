//
// Created by 严洪钧 on 2022/11/17.
//

#ifndef PROJECT_EXITABLE_FUNCTIONS_H
#define PROJECT_EXITABLE_FUNCTIONS_H
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

void error(const char *msg);
int e_socket(int domain, int type, int protocol);
int e_bind(int socket, const struct sockaddr *address, socklen_t address_len);
int e_accept(int socket, struct sockaddr *restrict address, socklen_t *restrict address_len);
struct hostent *e_gethostbyname(const char *name);
int e_connect(int socket, const struct sockaddr *address, socklen_t address_len);
ssize_t e_write(int fd, const void *buf, size_t nbyte);
ssize_t e_read(int fd, void *buf, size_t nbyte);

pid_t e_fork();

#endif //PROJECT_EXITABLE_FUNCTIONS_H
