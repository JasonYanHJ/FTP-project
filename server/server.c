#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../common/exitable_functions.h"
#include "../common/request.h"

void do_stuff(int);
void start_listen(int *socket, int port_no);

int main(int argc, char *argv[])
{
    signal(SIGCHLD, SIG_IGN);
    if (argc < 2) {
        fprintf(stderr,"usage %s port\n", argv[0]);
        exit(1);
    }

    int socket_control;
    start_listen(&socket_control, atoi(argv[1]));

    struct sockaddr_in cli_addr;
    socklen_t cli_len = 0;
    while (1) {
        int socket_data = e_accept(socket_control, (struct sockaddr *) &cli_addr, &cli_len);
        int pid = e_fork();
        if (pid == 0)  {
            close(socket_control);
            do_stuff(socket_data);
            exit(0);
        }
        else
            close(socket_data);
    }
    close(socket_control);
    return 0; /* we never get here */
}

void start_listen(int *socket, int port_no) {
    *socket = e_socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_no);
    e_bind(*socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    listen(*socket, 5);
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connection has been established.
 *****************************************/
void do_stuff (int sock)
{
    ssize_t n;
    char buffer[256];

    while (1) {
        try {
            bzero(buffer,256);
            n = e_read(sock,buffer,255);
            if (n == 0) break;
            printf("Here is the message(size=%ld): %s\n", n, buffer);
            struct request req;
            t_read_request(&req, buffer);
            printf("command: %s\narg1: %s\narg2: %s\narg3: %s\narg4: %s\n", req.command, req.args[0], req.args[1], req.args[2], req.args[3]);
            e_write(sock,"I got your request",18);
        }
        catch (err_no) {
            e_write(sock,"request syntax error!\n",21);
        }
    }
}
