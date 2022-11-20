#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../common/exitable_functions.h"
#include "../common/request.h"
#include "../common/path_helper.h"
#include "../common/socket_helper.h"
#include "../common/file_io_helper.h"

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

    getcwd(cwd, sizeof(cwd));

    while (1) {
        try {
            struct request req;
            n = e_read(socket_control, &req, sizeof(struct request));
            if (n == 0) break;

            // TODO: verify request
            e_write_code(socket_control, 0);

            int socket_data;
            connect_data_socket(&socket_data, socket_control);

            bzero(buffer,256);
            if (strcmp(req.command, "put") == 0) {
                char filename[1024];
                sprintf(filename, "%s/%s", cwd, req.args[1]);
                const char* open_mode = "wb";
                try {
                    t_expect_read_code(socket_control, 0);

                    FILE *fp = fopen(filename, open_mode);
                    ASSERT(fp != NULL, ERR_OPEN_REMOTE_FILE);

                    while ((n = e_read(socket_data, buffer, BUF_SIZE))) {
                        fwrite(buffer, n, sizeof(char), fp);
                    }
                    fclose(fp);

                    e_write_code(socket_control, 0);
                }
                catch(err_no) {
                    e_write_code(socket_control, err_no);
                }
            } else if (strcmp(req.command, "get") == 0) {
                char filename[1024];
                sprintf(filename, "%s/%s", cwd, req.args[0]);
                const char *open_mode = "rb";
                try {
                    FILE *fp = fopen(filename, open_mode);
                    e_write_code(socket_control, fp == NULL ? ERR_OPEN_REMOTE_FILE : 0);

                    t_expect_read_code(socket_control, 0);

                    _for_in_fp (BUF_SIZE, buffer_send, read_cnt, fp) {
                        e_write(socket_data, buffer_send, read_cnt);
                    }
                    fclose(fp);

                    e_write_code(socket_control, 0);
                }
                catch(err_no) {
                    e_write_code(socket_control, err_no);
                }
            } else if (strcmp(req.command, "ls") == 0) {
                char sys_call[1024];
                sprintf(sys_call, "ls -a %s >& %d 2>&1", cwd, socket_data);
                system(sys_call);
                e_write_code(socket_control, 0);
            } else if (strcmp(req.command, "cd") == 0) {
                try {
                    t_chdir(cwd, sizeof(cwd), req.args[0]);
                    e_write_code(socket_control, 0);
                }
                catch(err_no) {
                    e_write_code(socket_control, err_no);
                }
            } else if (strcmp(req.command, "pwd") == 0) {
                e_write(socket_data, cwd, strlen(cwd));
                e_write_code(socket_control, 0);
            } else if (strcmp(req.command, "mkdir") == 0) {
                try {
                    t_mkdir(req.args[0]);
                    e_write_code(socket_control, 0);
                }
                catch(err_no) {
                    e_write_code(socket_control, err_no);
                }
            } else if (strcmp(req.command, "delete") == 0) {
                try {
                    t_remove(req.args[0]);
                    e_write_code(socket_control, 0);
                }
                catch(err_no) {
                    e_write_code(socket_control, err_no);
                }
            }

            close(socket_data);
        }
        catch (err_no) {
            e_write(socket_control, &err_no, sizeof(err_no));
        }
    }
}
