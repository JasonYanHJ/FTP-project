#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "../common/exitable_functions.h"

struct sockaddr_in get_addr_from_host(int port_no, struct sockaddr_in *host_addr, const struct hostent *host);
void connect_to_server(int *socket_control, const char *name, int port_no);

int main(int argc, char *argv[])
{
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(1);
    }

    int socket_control;
    connect_to_server(&socket_control, argv[1], atoi(argv[2]));

    char buffer[256];
    while (1) {
        printf("Please enter the message: ");
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        if (strncmp(buffer, "quit", 4) == 0)
            break;
        e_write(socket_control, buffer, strlen(buffer));
        bzero(buffer,256);
        e_read(socket_control, buffer, 255);
        printf("%s\n",buffer);
    }
    close(socket_control);
    return 0;
}

void connect_to_server(int *socket_control, const char *name, int port_no) {
    *socket_control = e_socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *server = e_gethostbyname(name);
    struct sockaddr_in serv_addr = get_addr_from_host(port_no, &serv_addr, server);
    e_connect(*socket_control, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
}

struct sockaddr_in get_addr_from_host(int port_no, struct sockaddr_in *host_addr, const struct hostent *host) {
    bzero((char *) host_addr, sizeof((*host_addr)));
    (*host_addr).sin_family = AF_INET;
    bcopy((char *) host->h_addr,
          (char *) &(*host_addr).sin_addr.s_addr,
          host->h_length);
    (*host_addr).sin_port = htons(port_no);
    return (*host_addr);
}
