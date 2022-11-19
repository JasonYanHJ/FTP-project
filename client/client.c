#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../common/exitable_functions.h"
#include "../common/defines.h"
#include "../common/socket_helper.h"
#include "../common/try_catch.h"
#include "common/request.h"
#include "common/file_io_helper.h"

void create_data_socket(int *socket_data, int socket_control);

int main(int argc, char *argv[])
{
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(1);
    }

    int socket_control;
    e_socket_connect(&socket_control, e_get_server_addr(argv[1], atoi(argv[2])));

    char buffer[BUF_SIZE];
    while (1) {
        try {
            printf("Please enter the message: ");
            bzero(buffer, BUF_SIZE);
            fgets(buffer, BUF_SIZE - 1, stdin);
            if (strncmp(buffer, "quit", 4) == 0)
                break;

            // Turn request_string --> request_struct and send to server
            struct request req;
            t_read_request(&req, buffer);
            //TODO: verify request
            printf("command: %s\narg1: %s\narg2: %s\narg3: %s\narg4: %s\n", req.command, req.args[0], req.args[1], req.args[2], req.args[3]);
            e_write(socket_control, &req, sizeof(struct request));

            int ret_code;
            e_read(socket_control, &ret_code, sizeof(ret_code));
            ASSERT(ret_code == 0, ret_code)

            int socket_data;
            create_data_socket(&socket_data, socket_control);

            if(strcmp(req.command, "put") == 0){
                const char* open_mode = "rb";
                FILE* fp = fopen(req.args[0], open_mode);
                // TODO: What if fp is null?
                _for_in_fp(BUF_SIZE, buffer_send, read_cnt, fp){
                    e_write(socket_data, buffer_send, read_cnt);
                }
                fclose(fp);
                printf("put %s -> %s : OK\n", req.args[0], req.args[1]);
            }else if(strcmp(req.command, "get") == 0){
                const char* open_mode = "wb";
                FILE* fp = fopen(req.args[1], open_mode);

                ssize_t n;
                while ((n = e_read(socket_data, buffer, BUF_SIZE))) {
                    fwrite(buffer, n, sizeof(char), fp);
                }
                fclose(fp);
                printf("get %s -> %s : OK\n", req.args[0], req.args[1]);
            }else{
                bzero(buffer, BUF_SIZE);
                ssize_t n;
                while ((n = e_read(socket_data, buffer, BUF_SIZE - 1))) {
                    buffer[n] = '\0';
                    printf("%s(%lu)\n", buffer, n);
                }
            }

            close(socket_data);
        }
        catch(err_no) {
            printf("err_no: %d\n", err_no);
        }
    }

    close(socket_control);
    return 0;
}

void create_data_socket(int *socket_data, int socket_control) {
    int socket_listen;
    e_start_listen(&socket_listen, CLIENT_PORT);

    int ready = 1;
    e_write(socket_control, (char*) &ready, sizeof(ready));

    e_socket_accept(socket_data, socket_listen);
    close(socket_listen);
}
