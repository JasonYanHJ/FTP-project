#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "../common/exitable_functions.h"
#include "../common/request.h"
#include "../common/path_helper.h"
#include "../common/socket_helper.h"
#include "../common/file_io_helper.h"

void process_requests(int socket_control);
void connect_data_socket(int *socket_data, int socket_control);
void server_process_put(int socket_control, int socket_data, struct request *req, const char *cwd);
void server_process_get(int socket_control, int socket_data, struct request *req, const char *cwd);
void server_process_ls(int socket_control, int socket_data, const char *cwd);
void server_process_cd(int socket_control, struct request *req, char *cwd, ssize_t size);
void server_process_pwd(int socket_control, int socket_data, const char *cwd);
void server_process_mkdir(int socket_control, struct request *req);
void server_process_delete(int socket_control, struct request *req);

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
            process_requests(socket_control);
            printf("close connection\n");
            exit(0);
        }
        else
            close(socket_control);
    }

    close(socket_listen);
    return 0;
}


void process_requests (int socket_control)
{
    char cwd[1024];

    getcwd(cwd, sizeof(cwd));

    while (1) {
        try {
            struct request req;
            if (e_read(socket_control, &req, sizeof(struct request)) == 0)
                break;

            t_verify_request(&req);
            e_write_code(socket_control, SUC_REQ_ACCEPTED);

            int socket_data;
            connect_data_socket(&socket_data, socket_control);

            switch (req.cmd_no) {
                case CMD_PUT:
                    server_process_put(socket_control, socket_data, &req, cwd);
                    break;
                case CMD_GET:
                    server_process_get(socket_control, socket_data, &req, cwd);
                    break;
                case CMD_LS:
                    server_process_ls(socket_control, socket_data, cwd);
                    break;
                case CMD_CD:
                    server_process_cd(socket_control, &req, cwd, sizeof(cwd));
                    break;
                case CMD_PWD:
                    server_process_pwd(socket_control, socket_data, cwd);
                    break;
                case CMD_MKDIR:
                    server_process_mkdir(socket_control, &req);
                    break;
                case CMD_DELETE:
                    server_process_delete(socket_control, &req);
                    break;
            }

            close(socket_data);
        }
        catch (err_no) {
            e_write(socket_control, &err_no, sizeof(err_no));
        }
    }
}




void connect_data_socket(int *socket_data, int socket_control) {
    int ready;
    e_read(socket_control, &ready, sizeof ready);
    e_socket_connect(socket_data, e_get_peer_addr(socket_control));
}

void server_process_delete(int socket_control, struct request *req) {
    try {
        t_remove(req->args[0]);
        e_write_code(socket_control, SUC_REQ_DONE);
    }
    catch(err_no) {
        e_write_code(socket_control, err_no);
    }
}

void server_process_mkdir(int socket_control, struct request *req) {
    try {
        t_mkdir(req->args[0]);
        e_write_code(socket_control, SUC_REQ_DONE);
    }
    catch(err_no) {
        e_write_code(socket_control, err_no);
    }
}

void server_process_pwd(int socket_control, int socket_data, const char *cwd) {
    e_write(socket_data, cwd, strlen(cwd));
    e_write_code(socket_control, SUC_REQ_DONE);
}

void server_process_cd(int socket_control, struct request *req, char *cwd, ssize_t size) {
    try {
        t_chdir(cwd, size, req->args[0]);
        e_write_code(socket_control, SUC_REQ_DONE);
    }
    catch(err_no) {
        e_write_code(socket_control, err_no);
    }
}

void server_process_ls(int socket_control, int socket_data, const char *cwd) {
    char sys_call[1024];
    sprintf(sys_call, "ls -a %s >& %d 2>&1", cwd, socket_data);
    system(sys_call);
    e_write_code(socket_control, SUC_REQ_DONE);
}

void server_process_get(int socket_control, int socket_data, struct request *req, const char *cwd) {
    char filename[1024];
    merge_path(filename, cwd, req->args[0]);
    const char* open_mode = (strcmp(req->args[2], "ascii") == 0) ? "r+" : "rb+";
    try {
        FILE *fp = fopen(filename, open_mode);
        e_write_code(socket_control, fp == NULL ? ERR_OPEN_REMOTE_FILE : SUC_OPEN_REMOTE_FILE);

        t_expect_read_code(socket_control, SUC_OPEN_LOCAL_FILE);

        _for_in_fp (BUF_SIZE, buffer_send, read_cnt, fp) {
            e_write(socket_data, buffer_send, read_cnt);
        }
        fclose(fp);

        e_write_code(socket_control, SUC_REQ_DONE);
    }
    catch(err_no) {
        e_write_code(socket_control, err_no);
    }
}

void server_process_put(int socket_control, int socket_data, struct request *req, const char *cwd) {
    char buffer[BUF_SIZE];
    ssize_t n;
    char filename[1024];
    merge_path(filename, cwd, req->args[1]);
    const char* open_mode = (strcmp(req->args[2], "ascii") == 0) ? "w" : "wb";
    try {
        t_expect_read_code(socket_control, SUC_OPEN_LOCAL_FILE);

        FILE *fp = fopen(filename, open_mode);
        ASSERT(fp != NULL, ERR_OPEN_REMOTE_FILE);

        while ((n = e_read(socket_data, buffer, BUF_SIZE))) {
            fwrite(buffer, n, sizeof(char), fp);
        }
        fclose(fp);

        e_write_code(socket_control, SUC_REQ_DONE);
    }
    catch(err_no) {
        e_write_code(socket_control, err_no);
    }
}
