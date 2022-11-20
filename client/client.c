#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../common/exitable_functions.h"
#include "../common/defines.h"
#include "../common/socket_helper.h"
#include "common/request.h"
#include "common/file_io_helper.h"

void print_err_info(int err_no);
void create_data_socket(int *socket_data, int socket_control);
void client_process_put(int socket_control, int socket_data, struct request *req);
void client_process_get(int socket_control, int socket_data, struct request *req);

int main(int argc, char *argv[])
{
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(1);
    }

    int socket_control;
    e_socket_connect(&socket_control, e_get_server_addr(argv[1], atoi(argv[2])));

    char buffer[BUF_SIZE];
    struct request req;
    while (1) {
        // read and send one request to server
        try {
            printf("Please enter the message: ");
            bzero(buffer, BUF_SIZE);
            fgets(buffer, BUF_SIZE - 1, stdin);

            // Turn request_string --> request_struct and send to server
            t_read_request(&req, buffer);
            e_write(socket_control, &req, sizeof(struct request));

            t_expect_read_code(socket_control, SUC_REQ_ACCEPTED);

            // when request accepted, create one data socket to transfer data
            int socket_data;
            create_data_socket(&socket_data, socket_control);
            if (strcmp(req.command, "put") == 0) {
                client_process_put(socket_control, socket_data, &req);
            } else if (strcmp(req.command, "get") == 0){
                client_process_get(socket_control, socket_data, &req);
            } else {
                bzero(buffer, BUF_SIZE);
                ssize_t n;
                while ((n = e_read(socket_data, buffer, BUF_SIZE - 1))) {
                    buffer[n] = '\0';
                    printf("%s\n", buffer);
                }
            }
            close(socket_data);

            t_expect_read_code(socket_control, SUC_REQ_DONE);
        }
        catch(err_no) {
            if (err_no == SUC_REQ_QUIT)
                break;
            print_err_info(err_no);
        }

    }

    close(socket_control);
    return 0;
}


void print_err_info(int err_no) {
    switch (err_no) {
        case ERR_REQUEST_SYNTAX:
            printf("ERROR: ERR_REQUEST_SYNTAX\n");
            break;
        case ERR_UNKNOWN_CMD:
            printf("ERROR: ERR_UNKNOWN_CMD\n");
            break;
        case ERR_TOO_MANY_ARGS:
            printf("ERROR: ERR_TOO_MANY_ARGS\n");
            break;
        case ERR_TOO_FEW_ARGS:
            printf("ERROR: ERR_TOO_FEW_ARGS\n");
            break;
        case ERR_INVALID_ARGS:
            printf("ERROR: ERR_INVALID_ARGS\n");
            break;
        case ERR_OPEN_LOCAL_FILE:
            printf("ERROR: ERR_OPEN_LOCAL_FILE\n");
            break;
        case ERR_OPEN_REMOTE_FILE:
            printf("ERROR: ERR_OPEN_REMOTE_FILE\n");
            break;
        case ERR_DELETE:
            printf("ERROR: ERR_DELETE\n");
            break;
        case ERR_MKDIR:
            printf("ERROR: ERR_MKDIR\n");
            break;
        case ERR_CHDIR:
            printf("ERROR: ERR_CHDIR\n");
            break;
        default:
            break;
    }
}

void client_process_get(int socket_control, int socket_data, struct request *req) {
    char buffer[BUF_SIZE];
    const char* open_mode = (strcmp(req->args[2], "ascii") == 0) ? "w" : "wb";
    try {
        t_expect_read_code(socket_control, SUC_OPEN_REMOTE_FILE);

        FILE *fp = fopen((*req).args[1], open_mode);
        ASSERT(fp != NULL, ERR_OPEN_LOCAL_FILE);
        e_write_code(socket_control, SUC_OPEN_LOCAL_FILE);

        ssize_t n;
        while ((n = e_read(socket_data, buffer, BUF_SIZE))) {
            fwrite(buffer, n, sizeof(char), fp);
        }
        fclose(fp);
    }
    catch(err_no) {
        e_write_code(socket_control, err_no);
    }
}

void client_process_put(int socket_control, int socket_data, struct request *req) {
    const char* open_mode = (strcmp(req->args[2], "ascii") == 0) ? "r+" : "rb+";
    try {
        FILE *fp = fopen((*req).args[0], open_mode);
        ASSERT(fp != NULL, ERR_OPEN_LOCAL_FILE);
        e_write_code(socket_control, SUC_OPEN_LOCAL_FILE);

        _for_in_fp (BUF_SIZE, buffer_send, read_cnt, fp) {
            e_write(socket_data, buffer_send, read_cnt);
        }
        fclose(fp);
    }
    catch(err_no) {
        e_write_code(socket_control, err_no);
    }
}

void create_data_socket(int *socket_data, int socket_control) {
    int socket_listen;
    e_start_listen(&socket_listen, CLIENT_PORT);

    int ready = 1;
    e_write(socket_control, (char*) &ready, sizeof(ready));

    e_socket_accept(socket_data, socket_listen);
    close(socket_listen);
}
