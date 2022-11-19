#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../common/exitable_functions.h"
#include "../common/request.h"
#include "../common/path_helper.h"
#include "../common/socket_helper.h"

void do_stuff(int, const char *);

int main(int argc, char *argv[])
{
    signal(SIGCHLD, SIG_IGN);
    if (argc < 2) {
        fprintf(stderr,"usage %s port\n", argv[0]);
        exit(1);
    }

    int socket_listen;
    e_start_listen(&socket_listen, atoi(argv[1]));

    while (1) {
        int socket_control;
        e_socket_accept(&socket_control, socket_listen);

        int pid = e_fork();
        if (pid == 0)  {
            close(socket_listen);
            do_stuff(socket_control, argv[0]);
            printf("close connection\n");
            exit(0);
        }
        else
            close(socket_control);
    }

    close(socket_listen);
    return 0;
}

void connect_data_socket(int *socket_data, int socket_control)
{
    int ready;
    e_read(socket_control, &ready, sizeof ready);
    e_socket_connect(socket_data, e_get_peer_addr(socket_control));
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connection has been established.
 *****************************************/
void do_stuff (int socket_control, const char *path)
{
    ssize_t n;
    char buffer[256];
    char cwd[1024];

    strcpy(cwd, path);
    path_back(cwd);
    printf("cwd: %s\n", cwd);

    while (1) {
        try {
            bzero(buffer,256);
            n = e_read(socket_control, buffer, 255);
            if (n == 0) break;
            printf("Here is the message(size=%ld): %s\n", n, buffer);
            struct request req;
            t_read_request(&req, buffer);
            // TODO: verify request
            printf("command: %s\narg1: %s\narg2: %s\narg3: %s\narg4: %s\n", req.command, req.args[0], req.args[1], req.args[2], req.args[3]);

            int accepted = 0;
            e_write(socket_control, &accepted, sizeof(accepted));

            int socket_data;
            connect_data_socket(&socket_data, socket_control);

            if (strcmp(req.command, "ls") == 0) {
                char sys_call[1024];
                sprintf(sys_call, "ls -a %s >& %d 2>&1", cwd, socket_data);
                system(sys_call);
            } else if (strcmp(req.command, "cd") == 0) {
                if (strcmp(req.args[0], "..") == 0)
                    path_back(cwd);
                else
                    path_add(cwd, req.args[0]);
                e_write(socket_data, "success", 7);
            } else if (strcmp(req.command, "pwd") == 0) {
                e_write(socket_data, cwd, strlen(cwd));
            }

            close(socket_data);
        }
        catch (err_no) {
            e_write(socket_control, &err_no, sizeof(err_no));
        }
    }
}
